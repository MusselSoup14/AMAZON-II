/*
 * (C) Copyright 2001
 * Denis Peter, MPL AG Switzerland
 *
 * For BBB support (C) Copyright 2003
 * Gary Jennejohn, DENX Software Engineering <gj@denx.de>
 *
 * Most of this source has been derived from the Linux USB
 * project. BBB support based on /sys/dev/usb/umass.c from
 * FreeBSD.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

/* Note:
 * Currently only the CBI transport protocoll has been implemented, and it
 * is only tested with a TEAC USB Floppy. Other Massstorages with CBI or CB
 * transport protocoll may work as well.
 */
/*
 * New Note:
 * Support for USB Mass Storage Devices (BBB) has been added. It has
 * only been tested with USB memory sticks.
 * Nota bene: if you are using the BBB support with a little-endian
 * CPU then you MUST define LITTLEENDIAN in the configuration file!
 */

#include <sdk.h>


//#define USB_STOR_DEBUG
#undef BBB_COMDAT_TRACE
#undef BBB_XPORT_TRACE

#ifdef	USB_STOR_DEBUG
#define USB_STOR_PRINTF(fmt,args...)	debugprintf (fmt ,##args)
#else
#define USB_STOR_PRINTF(fmt,args...)
#endif

#include <usb_host/scsi.h>
/* direction table -- this indicates the direction of the data
 * transfer for each command code -- a 1 indicates input
 */
unsigned char us_direction[256 / 8] =
{
    0x28, 0x81, 0x14, 0x14, 0x20, 0x01, 0x90, 0x77,
    0x0C, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define US_DIRECTION(x) ((us_direction[x>>3] >> (x & 7)) & 1)

static unsigned char usb_stor_buf[512];
static ccb usb_ccb;

extern int usbhostinit( void );

/*
 * CBI style
 */

#define US_CBI_ADSC		0

/*
 * BULK only
 */
#define US_BBB_RESET		0xff
#define US_BBB_GET_MAX_LUN	0xfe

/* Command Block Wrapper */
#define CBWSIGNATURE	0x43425355
#define CBWFLAGS_OUT	0x00
#define CBWFLAGS_IN	0x80
#define CBWCDBLENGTH	16

typedef struct
{
    __u32		dCBWSignature;
    __u32		dCBWTag;
    __u32		dCBWDataTransferLength;
    __u8		bCBWFlags;
    __u8		bCBWLUN;
    __u8		bCDBLength;
    __u8		CBWCDB[CBWCDBLENGTH];
} umass_bbb_cbw_t;
#define UMASS_BBB_CBW_SIZE	31
static __u32 CBWTag = 0x81A088A8;

/* Command Status Wrapper */
#define CSWSIGNATURE	0x53425355
#define CSWSTATUS_GOOD	0x0
#define CSWSTATUS_FAILED 	0x1
#define CSWSTATUS_PHASE	0x2
typedef struct
{
    __u32		dCSWSignature;
    __u32		dCSWTag;
    __u32		dCSWDataResidue;
    __u8		bCSWStatus;
} umass_bbb_csw_t;
#define UMASS_BBB_CSW_SIZE	13

// #define USB_MAX_STOR_DEV  3 
static int usb_max_devs; /* number of highest available usb device */

static block_dev_desc_t usb_dev_desc[USB_MAX_STOR_DEV];

struct us_data;
typedef int ( *trans_cmnd )( ccb*, struct us_data* );
typedef int ( *trans_reset )( struct us_data* );

struct us_data
{
    struct usb_device	*pusb_dev;	 /* this usb_device */
    unsigned int		flags;		 /* from filter initially */
    unsigned char		ifnum;		 /* interface number */
    unsigned char		ep_in;		 /* in endpoint */
    unsigned char		ep_out;		 /* out ....... */
    unsigned char		ep_int;		 /* interrupt . */
    unsigned char		subclass;	 /* as in overview */
    unsigned char		protocol;	 /* .............. */
    unsigned char		attention_done;	 /* force attn on first cmd */
    unsigned short	ip_data;	 /* interrupt data */
    int					action;		 /* what to do */
    int					ip_wanted; /* needed */
    int					*irq_handle;	 /* for USB int requests */
    unsigned int		irqpipe;	 /* pipe for release_irq */
    unsigned char		irqmaxp;	/* max packed for irq Pipe */
    unsigned char	irqinterval; /* Intervall for IRQ Pipe */
    ccb							*srb;		 /* current srb */
    trans_reset			transport_reset; /* reset routine */
    trans_cmnd			transport; /* transport routine */
};

static struct us_data usb_stor[USB_MAX_STOR_DEV];

static unsigned char sense_buff[64];

static  block_dev_desc_t *cur_dev = NULL;
static struct usb_device *curdev = NULL;

#define USB_STOR_TRANSPORT_GOOD	   0
#define USB_STOR_TRANSPORT_FAILED -1
#define USB_STOR_TRANSPORT_ERROR  -2

int usb_stor_get_info( struct usb_device *dev, struct us_data *us, block_dev_desc_t *dev_desc );
int usb_storage_probe( struct usb_device *dev, unsigned int ifnum, struct us_data *ss );
unsigned long usb_stor_read( int device, unsigned long blknr, unsigned long blkcnt, unsigned long *buffer );
unsigned long usb_stor_write( int device, unsigned long blknr, unsigned long blkcnt, unsigned long *buffer );

block_dev_desc_t *usb_stor_get_dev( int index )
{
    return &usb_dev_desc[index];
}

void usb_show_progress( void )
{
    debugstring( "." );
}

#define DEFAULT_SECTOR_SIZE	512
#define DOS_PART_TBL_OFFSET	0x1be
#define DOS_PART_MAGIC_OFFSET	0x1fe
#define DOS_PBR_FSTYPE_OFFSET	0x36
#define DOS_PBR_MEDIA_TYPE_OFFSET	0x15
#define DOS_MBR	0
#define DOS_PBR	1

void dev_print ( block_dev_desc_t *dev_desc )
{
#ifdef CONFIG_LBA48
    uint64_t lba512; /* number of blocks if 512bytes block size */
#else
    lbaint_t lba512;
#endif

    if ( dev_desc->type == DEV_TYPE_UNKNOWN )
    {
        debugprintf ( "not available\r\n" );
        return;
    }
    if ( dev_desc->if_type == IF_TYPE_SCSI )
    {
        debugprintf ( "(%d:%d) ", dev_desc->target, dev_desc->lun );
    }
    if ( dev_desc->if_type == IF_TYPE_IDE )
    {
        debugprintf ( "Model: %s Firm: %s Ser#: %s\r\n",
                      dev_desc->vendor,
                      dev_desc->revision,
                      dev_desc->product );
    }
    else
    {
        debugprintf ( "Vendor: %s Prod.: %s Rev: %s\r\n",
                      dev_desc->vendor,
                      dev_desc->product,
                      dev_desc->revision );
    }
    debugstring( "            Type: " );
    if ( dev_desc->removable )
        debugstring( "Removable " );
    switch ( dev_desc->type & 0x1F )
    {
    case DEV_TYPE_HARDDISK:
        debugstring ( "Hard Disk" );
        break;
    case DEV_TYPE_CDROM:
        debugstring ( "CD ROM" );
        break;
    case DEV_TYPE_OPDISK:
        debugstring ( "Optical Device" );
        break;
    case DEV_TYPE_TAPE:
        debugstring ( "Tape" );
        break;
    default:
        debugprintf ( "# %02X #", dev_desc->type & 0x1F );
        break;
    }
    debugstring ( "\r\n" );
    if ( ( dev_desc->lba * dev_desc->blksz ) > 0L )
    {
        ulong mb, mb_quot, mb_rem, gb, gb_quot, gb_rem;
        lbaint_t lba;

        lba = dev_desc->lba;

        lba512 = ( lba * ( dev_desc->blksz / 512 ) );
        mb = ( 10 * lba512 ) / 2048;	/* 2048 = (1024 * 1024) / 512 MB */
        /* round to 1 digit */
        mb_quot	= mb / 10;
        mb_rem	= mb - ( 10 * mb_quot );

        gb = mb / 1024;
        gb_quot	= gb / 10;
        gb_rem	= gb - ( 10 * gb_quot );
#ifdef CONFIG_LBA48
        if ( dev_desc->lba48 )
            debugprintf ( "            Supports 48-bit addressing\r\n" );
#endif
#if defined(CFG_64BIT_LBA) && defined(CFG_64BIT_VSPRINTF)
        debugprintf ( "            Capacity: %ld.%ld MB = %ld.%ld GB (%qd x %ld)\r\n",
                      mb_quot, mb_rem,
                      gb_quot, gb_rem,
                      lba,
                      dev_desc->blksz );
#else
        debugprintf ( "            Capacity: %ld.%ld MB = %ld.%ld GB (%ld x %ld)\r\n",
                      mb_quot, mb_rem,
                      gb_quot, gb_rem,
                      ( ulong )lba,
                      dev_desc->blksz );
#endif
    }
    else
    {
        debugstring ( "            Capacity: not available\r\n" );
    }
}


int test_part_dos ( block_dev_desc_t *dev_desc )
{
    unsigned char buffer[DEFAULT_SECTOR_SIZE];
    dcache_invalidate_way();
    if ( ( dev_desc->block_read( dev_desc->dev, 0, 1, ( ulong * ) buffer ) != 1 ) ||
         ( buffer[DOS_PART_MAGIC_OFFSET + 0] != 0x55 ) ||
         ( buffer[DOS_PART_MAGIC_OFFSET + 1] != 0xaa ) )
    {
        return ( -1 );
    }
    return ( 0 );
}

void init_part ( block_dev_desc_t * dev_desc )
{
    if ( test_part_dos( dev_desc ) == 0 )
    {
        dev_desc->part_type = PART_TYPE_DOS;
        return;
    }

}

/*********************************************************************************
 * (re)-scan the usb and reports device info
 * to the user if mode = 1
 * returns current device or -1 if no
 */
int usb_stor_scan( int mode )
{
    unsigned char i;
    struct usb_device *dev;

    /* GJ */
    memset( usb_stor_buf, 0, sizeof( usb_stor_buf ) );

    if( mode == 1 )
    {
        debugprintf( "       scanning bus for storage devices...\r\n" );
    }
    usb_disable_asynch( 1 ); /* asynch transfer not allowed */

    for( i = 0; i < USB_MAX_STOR_DEV; i++ )
    {
        memset( &usb_dev_desc[i], 0, sizeof( block_dev_desc_t ) );
        usb_dev_desc[i].target = 0xff;
        usb_dev_desc[i].if_type = IF_TYPE_USB;
        usb_dev_desc[i].dev = i;
        usb_dev_desc[i].part_type = PART_TYPE_UNKNOWN;
        usb_dev_desc[i].block_read = usb_stor_read;
        usb_dev_desc[i].block_write = usb_stor_write;
    }
    usb_max_devs = 0;
    for( i = 0; i < USB_MAX_DEVICE; i++ )
    {
        dev = usb_get_dev_index( i ); /* get device */
        USB_STOR_PRINTF( "dev=0x%X\r\n", ( int )dev );
        USB_STOR_PRINTF( "i=%d\r\n", i );
        if( dev == NULL )
        {
            break; /* no more devices avaiable */
        }
        curdev = dev;
        if( usb_storage_probe( dev, 0, &usb_stor[usb_max_devs] ) ) /* ok, it is a storage devices */
        {
            /* get info and fill it in */

            if( usb_stor_get_info( dev, &usb_stor[usb_max_devs], &usb_dev_desc[usb_max_devs] ) )
            {
                if( mode == 1 )
                {
                    debugprintf ( "  Device %d: ", usb_max_devs );
                    dev_print( &usb_dev_desc[usb_max_devs] );
                } /* if mode */
                usb_max_devs++;
            } /* if get info ok */
        } /* if storage device */
        if( usb_max_devs == USB_MAX_STOR_DEV )
        {
            debugprintf( "max USB Storage Device reached: %d stopping\r\n", usb_max_devs );
            break;
        }
    } /* for */
    usb_disable_asynch( 0 ); /* asynch transfer allowed */
    if( usb_max_devs > 0 )
        return 0;
    else
        return -1;
}

static int usb_stor_irq( struct usb_device *dev )
{
    struct us_data *us;
    us = ( struct us_data * )dev->privptr;

    if( us->ip_wanted )
    {
        us->ip_wanted = 0;
    }
    return 0;
}

#ifdef	USB_STOR_DEBUG

static void usb_show_srb( ccb * pccb )
{
    int i;
    debugprintf( "SRB: len %d datalen 0x%lX\r\n ", pccb->cmdlen, pccb->datalen );
    for( i = 0; i < 12; i++ )
    {
        debugprintf( "%02X ", pccb->cmd[i] );
    }
    debugprintf( "\r\n" );
}

static void display_int_status( unsigned long tmp )
{
    debugprintf( "Status: %s %s %s %s %s %s %s\r\n",
                 ( tmp & USB_ST_ACTIVE ) ? "Active" : "",
                 ( tmp & USB_ST_STALLED ) ? "Stalled" : "",
                 ( tmp & USB_ST_BUF_ERR ) ? "Buffer Error" : "",
                 ( tmp & USB_ST_BABBLE_DET ) ? "Babble Det" : "",
                 ( tmp & USB_ST_NAK_REC ) ? "NAKed" : "",
                 ( tmp & USB_ST_CRC_ERR ) ? "CRC Error" : "",
                 ( tmp & USB_ST_BIT_ERR ) ? "Bitstuff Error" : "" );
}
#endif
/***********************************************************************
 * Data transfer routines
 ***********************************************************************/

static int us_one_transfer( struct us_data *us, int pipe, char *buf, int length )
{
    int max_size;
    int this_xfer;
    int result;
    int partial;
    int maxtry;
    int stat;

    /* determine the maximum packet size for these transfers */
    max_size = usb_maxpacket( us->pusb_dev, pipe ) * 16;

    /* while we have data left to transfer */
    while ( length )
    {

        /* calculate how long this will be -- maximum or a remainder */
        this_xfer = length > max_size ? max_size : length;
        length -= this_xfer;

        /* setup the retry counter */
        maxtry = 10;

        /* set up the transfer loop */
        do
        {
            /* transfer the data */
            USB_STOR_PRINTF( "Bulk xfer 0x%x(%d) try #%d\r\n",
                             ( unsigned int )buf, this_xfer, 11 - maxtry );
            result = usb_bulk_msg( us->pusb_dev, pipe, buf,
                                   this_xfer, &partial, USB_CNTL_TIMEOUT * 5 );
            USB_STOR_PRINTF( "bulk_msg returned %d xferred %d/%d\r\n",
                             result, partial, this_xfer );
            if( us->pusb_dev->status != 0 )
            {
                /* if we stall, we need to clear it before we go on */
#ifdef USB_STOR_DEBUG
                display_int_status( us->pusb_dev->status );
#endif
                if ( us->pusb_dev->status & USB_ST_STALLED )
                {
                    USB_STOR_PRINTF( "stalled ->clearing endpoint halt for pipe 0x%x\r\n", pipe );
                    stat = us->pusb_dev->status;
                    usb_clear_halt( us->pusb_dev, pipe );
                    us->pusb_dev->status = stat;
                    if( this_xfer == partial )
                    {
                        USB_STOR_PRINTF( "bulk transferred with error %X, but data ok\r\n", us->pusb_dev->status );
                        return 0;
                    }
                    else
                        return result;
                }
                if ( us->pusb_dev->status & USB_ST_NAK_REC )
                {
                    USB_STOR_PRINTF( "Device NAKed bulk_msg\r\n" );
                    return result;
                }
                if( this_xfer == partial )
                {
                    USB_STOR_PRINTF( "bulk transferred with error %d, but data ok\r\n", us->pusb_dev->status );
                    return 0;
                }
                /* if our try counter reaches 0, bail out */
                USB_STOR_PRINTF( "bulk transferred with error %d, data %d\r\n", us->pusb_dev->status, partial );
                if ( !maxtry-- )
                    return result;
            }
            /* update to show what data was transferred */
            this_xfer -= partial;
            buf += partial;
            /* continue until this transfer is done */
        }
        while ( this_xfer );
    }

    /* if we get here, we're done and successful */
    return 0;
}

static int usb_stor_BBB_reset( struct us_data *us )
{
    int result;
    unsigned int pipe;

    /*
     * Reset recovery (5.3.4 in Universal Serial Bus Mass Storage Class)
     *
     * For Reset Recovery the host shall issue in the following order:
     * a) a Bulk-Only Mass Storage Reset
     * b) a Clear Feature HALT to the Bulk-In endpoint
     * c) a Clear Feature HALT to the Bulk-Out endpoint
     *
     * This is done in 3 steps.
     *
     * If the reset doesn't succeed, the device should be port reset.
     *
     * This comment stolen from FreeBSD's /sys/dev/usb/umass.c.
     */
    USB_STOR_PRINTF( "BBB_reset\r\n" );
    result = usb_control_msg( us->pusb_dev, usb_sndctrlpipe( us->pusb_dev, 0 ),
                              US_BBB_RESET, USB_TYPE_CLASS | USB_RECIP_INTERFACE,
                              0, us->ifnum, 0, 0, USB_CNTL_TIMEOUT * 5 );
    if( ( result < 0 ) && ( us->pusb_dev->status & USB_ST_STALLED ) )
    {
        USB_STOR_PRINTF( "RESET:stall\r\n" );
        return -1;
    }
    /* long wait for reset */
    wait_ms( 150 );
    USB_STOR_PRINTF( "BBB_reset result %d: status %X reset\r\n", result, us->pusb_dev->status );
    pipe = usb_rcvbulkpipe( us->pusb_dev, us->ep_in );
    result = usb_clear_halt( us->pusb_dev, pipe );
    /* long wait for reset */
    wait_ms( 150 );
    USB_STOR_PRINTF( "BBB_reset result %d: status %X clearing IN endpoint\r\n", result, us->pusb_dev->status );
    /* long wait for reset */
    pipe = usb_sndbulkpipe( us->pusb_dev, us->ep_out );
    result = usb_clear_halt( us->pusb_dev, pipe );
    wait_ms( 150 );
    USB_STOR_PRINTF( "BBB_reset result %d: status %X clearing OUT endpoint\r\n", result, us->pusb_dev->status );
    USB_STOR_PRINTF( "BBB_reset done\r\n" );
    return 0;
}

/* FIXME: this reset function doesn't really reset the port, and it
 * should. Actually it should probably do what it's doing here, and
 * reset the port physically
 */
static unsigned char reset_cmd[12];
static int usb_stor_CB_reset( struct us_data *us )
{
    //unsigned char cmd[12];
    int result;

    USB_STOR_PRINTF( "CB_reset\r\n" );
    memset( reset_cmd, 0xFF, sizeof( reset_cmd ) );
    reset_cmd[0] = SCSI_SEND_DIAG;
    reset_cmd[1] = 4;
    result = usb_control_msg( us->pusb_dev, usb_sndctrlpipe( us->pusb_dev, 0 ),
                              US_CBI_ADSC, USB_TYPE_CLASS | USB_RECIP_INTERFACE,
                              0, us->ifnum, reset_cmd, sizeof( reset_cmd ), USB_CNTL_TIMEOUT * 5 );

    /* long wait for reset */
    wait_ms( 1500 );
    USB_STOR_PRINTF( "CB_reset result %d: status %X clearing endpoint halt\r\n", result, us->pusb_dev->status );
    usb_clear_halt( us->pusb_dev, usb_rcvbulkpipe( us->pusb_dev, us->ep_in ) );
    usb_clear_halt( us->pusb_dev, usb_rcvbulkpipe( us->pusb_dev, us->ep_out ) );

    USB_STOR_PRINTF( "CB_reset done\r\n" );
    return 0;
}

/*
 * Set up the command for a BBB device. Note that the actual SCSI
 * command is copied into cbw.CBWCDB.
 */

int usb_stor_BBB_comdat( int dir_in, ccb *srb, struct us_data *us )
{
    int result;
    int actlen;
//	int dir_in; // 매개 변수로 변경 
    unsigned int pipe;
    umass_bbb_cbw_t cbw;
//	dir_in = US_DIRECTION(srb->cmd[0]); // 매개 변수에서 가져와서 처리 

#ifdef BBB_COMDAT_TRACE
    debugprintf( "dir %d lun %d cmdlen %d cmd %p datalen %d pdata %p\r\n", dir_in, srb->lun, srb->cmdlen, srb->cmd, srb->datalen, srb->pdata );
    if ( srb->cmdlen )
    {
        for( result = 0; result < srb->cmdlen; result++ )
            debugprintf( "cmd[%d] %#x ", result, srb->cmd[result] );
        debugprintf( "\r\n" );
    }
#endif
    /* sanity checks */
#if 0 
    if ( !( srb->cmdlen <= CBWCDBLENGTH ) )
    {
        USB_STOR_PRINTF( "usb_stor_BBB_comdat:cmdlen too large\r\n" );
        return -1;
    }
#endif 

    /* always OUT to the ep */
    pipe = usb_sndbulkpipe( us->pusb_dev, us->ep_out );

    cbw.dCBWSignature = swap_32( CBWSIGNATURE );
    cbw.dCBWTag = swap_32( CBWTag++ );
    cbw.dCBWDataTransferLength = swap_32( srb->datalen );
    cbw.bCBWFlags = ( dir_in ? CBWFLAGS_IN : CBWFLAGS_OUT );
    cbw.bCBWLUN = srb->lun;
    cbw.bCDBLength = srb->cmdlen;
    /* copy the command data into the CBW command data buffer */
    /* DST SRC LEN!!! */

    memcpy( cbw.CBWCDB, srb->cmd, srb->cmdlen );
    dcache_invalidate_way();
    result = usb_bulk_msg( us->pusb_dev, pipe, &cbw, UMASS_BBB_CBW_SIZE, &actlen, USB_CNTL_TIMEOUT * 5 );

#if 0 // debug
    int i;
    __u8 *ttemp = ( __u8 * )&cbw;
    debugstring( "__________________ command \r\n" );
    for( i = 0; i < UMASS_BBB_CBW_SIZE; i++ )
    {
        debugprintf( "%02x ", ttemp[i] );
    }
    debugstring( "\r\n\n\n" );
#endif 

#if 0 // 디버그시에만 필요할 것 같음
    if ( result < 0 )
        USB_STOR_PRINTF( "usb_stor_BBB_comdat:usb_bulk_msg error\r\n" );
#endif 
    return result;
}

/* FIXME: we also need a CBI_command which sets up the completion
 * interrupt, and waits for it
 */
int usb_stor_CB_comdat( ccb *srb, struct us_data *us )
{
    int result;
    int dir_in, retry;
    unsigned int pipe;
    unsigned long status;

    retry = 5;
    dir_in = US_DIRECTION( srb->cmd[0] );

    if( dir_in )
        pipe = usb_rcvbulkpipe( us->pusb_dev, us->ep_in );
    else
        pipe = usb_sndbulkpipe( us->pusb_dev, us->ep_out );
    while( retry-- )
    {
        USB_STOR_PRINTF( "CBI gets a command: Try %d\r\n", 5 - retry );
#ifdef USB_STOR_DEBUG
        usb_show_srb( srb );
#endif
        /* let's send the command via the control pipe */
        result = usb_control_msg( us->pusb_dev, usb_sndctrlpipe( us->pusb_dev, 0 ),
                                  US_CBI_ADSC, USB_TYPE_CLASS | USB_RECIP_INTERFACE,
                                  0, us->ifnum,
                                  srb->cmd, srb->cmdlen, USB_CNTL_TIMEOUT * 5 );
        USB_STOR_PRINTF( "CB_transport: control msg returned %d, status %X\r\n", result, us->pusb_dev->status );
        /* check the return code for the command */
        if ( result < 0 )
        {
            if( us->pusb_dev->status & USB_ST_STALLED )
            {
                status = us->pusb_dev->status;
                USB_STOR_PRINTF( " stall during command found, clear pipe\r\n" );
                usb_clear_halt( us->pusb_dev,  usb_sndctrlpipe( us->pusb_dev, 0 ) );
                us->pusb_dev->status = status;
            }
            USB_STOR_PRINTF( " error during command %02X Stat = %X\r\n", srb->cmd[0], us->pusb_dev->status );
            return result;
        }
        /* transfer the data payload for this command, if one exists*/

        USB_STOR_PRINTF( "CB_transport: control msg returned %d, direction is %s to go 0x%lx\r\n", result, dir_in ? "IN" : "OUT", srb->datalen );
        if ( srb->datalen )
        {
            result = us_one_transfer( us, pipe, srb->pdata, srb->datalen );
            USB_STOR_PRINTF( "CBI attempted to transfer data, result is %d status %lX, len %d\r\n", result, us->pusb_dev->status, us->pusb_dev->act_len );
            if( !( us->pusb_dev->status & USB_ST_NAK_REC ) )
                break;
        } /* if (srb->datalen) */
        else
            break;
    }
    /* return result */

    return result;
}

int usb_stor_CBI_get_status ( ccb * srb, struct us_data *us )
{
    int timeout;

    us->ip_wanted = 1;
    submit_int_msg ( us->pusb_dev, us->irqpipe,
                     ( void * ) &us->ip_data, us->irqmaxp, us->irqinterval );
    timeout = 1000;
    while ( timeout-- )
    {
        if ( ( volatile int * ) us->ip_wanted == 0 )
            break;
        wait_ms ( 10 );
    }
    if ( us->ip_wanted )
    {
        debugprintf ( "	Did not get interrupt on CBI\r\n" );
        us->ip_wanted = 0;
        return USB_STOR_TRANSPORT_ERROR;
    }
    USB_STOR_PRINTF
    ( "Got interrupt data 0x%x, transfered %d status 0x%lX\r\n",
      us->ip_data, us->pusb_dev->irq_act_len,
      us->pusb_dev->irq_status );
    /* UFI gives us ASC and ASCQ, like a request sense */
    if ( us->subclass == US_SC_UFI )
    {
        if ( srb->cmd[0] == SCSI_REQ_SENSE ||
             srb->cmd[0] == SCSI_INQUIRY )
            return USB_STOR_TRANSPORT_GOOD; /* Good */
        else if ( us->ip_data )
            return USB_STOR_TRANSPORT_FAILED;
        else
            return USB_STOR_TRANSPORT_GOOD;
    }
    /* otherwise, we interpret the data normally */
    switch ( us->ip_data )
    {
    case 0x0001:
        return USB_STOR_TRANSPORT_GOOD;
    case 0x0002:
        return USB_STOR_TRANSPORT_FAILED;
    default:
        return USB_STOR_TRANSPORT_ERROR;
    }			/* switch */
    return USB_STOR_TRANSPORT_ERROR;
}

#define USB_TRANSPORT_UNKNOWN_RETRY 5
#define USB_TRANSPORT_NOT_READY_RETRY 10

/* clear a stall on an endpoint - special for BBB devices */
int usb_stor_BBB_clear_endpt_stall( struct us_data *us, __u8 endpt )
{
    int result;

    /* ENDPOINT_HALT = 0, so set value to 0 */
    result = usb_control_msg( us->pusb_dev, usb_sndctrlpipe( us->pusb_dev, 0 ),
                              USB_REQ_CLEAR_FEATURE, USB_RECIP_ENDPOINT,
                              0, endpt, 0, 0, USB_CNTL_TIMEOUT * 5 );
    return result;
}

umass_bbb_csw_t csw;
int usb_stor_BBB_transport( ccb *srb, struct us_data *us )
{
    int result, retry;
    int dir_in;
    int actlen, data_actlen;
    unsigned int pipe, pipein, pipeout;
    //umass_bbb_csw_t csw;
#ifdef BBB_XPORT_TRACE
    unsigned char *ptr;
    int index;
#endif
    dcache_invalidate_way();
    dir_in = US_DIRECTION( srb->cmd[0] );

    /* COMMAND phase */
    USB_STOR_PRINTF( "COMMAND phase\r\n" );
    result = usb_stor_BBB_comdat( dir_in, srb, us );
    if ( result < 0 )
    {
        USB_STOR_PRINTF( "failed to send CBW status %ld\r\n", us->pusb_dev->status );
        usb_stor_BBB_reset( us );
        return USB_STOR_TRANSPORT_FAILED;
    }
    
    pipein = usb_rcvbulkpipe( us->pusb_dev, us->ep_in );
    pipeout = usb_sndbulkpipe( us->pusb_dev, us->ep_out );
    /* DATA phase + error handling */
    data_actlen = 0;
    /* no data, go immediately to the STATUS phase */
    if ( srb->datalen == 0 )
        goto st;
    USB_STOR_PRINTF( "DATA phase\r\n" );
    if ( dir_in )
        pipe = pipein;
    else
        pipe = pipeout;
    USB_STOR_PRINTF( "pipe = 0x%x\r\n", pipe );

    result = usb_bulk_msg( us->pusb_dev, pipe, srb->pdata, srb->datalen, &data_actlen, USB_CNTL_TIMEOUT * 5 );
#if 0 // debug
    int i;
    debugstring( "_________________read data\r\n" );
    for( i = 0; i < data_actlen; i++ )
    {
        debugprintf( "%02x ", srb->pdata[i] );
    }
    debugstring( "\r\n\n\n" );
#endif 

    /* special handling of STALL in DATA phase */
    if( ( result < 0 ) && ( us->pusb_dev->status & USB_ST_STALLED ) )
    {
        USB_STOR_PRINTF( "DATA:stall\r\n" );
        /* clear the STALL on the endpoint */
        result = usb_stor_BBB_clear_endpt_stall( us, dir_in ? us->ep_in : us->ep_out );
        if ( result >= 0 )
            /* continue on to STATUS phase */
            goto st;
    }
    if ( result < 0 )
    {
        USB_STOR_PRINTF( "usb_bulk_msg error status %ld\r\n",
                         us->pusb_dev->status );
        usb_stor_BBB_reset( us );
        return USB_STOR_TRANSPORT_FAILED;
    }
#ifdef BBB_XPORT_TRACE
    for ( index = 0; index < data_actlen; index++ )
        debugprintf( "pdata[%d] %#x ", index, srb->pdata[index] );
    debugprintf( "\r\n" );
#endif
    /* STATUS phase + error handling */
st:
    retry = 0;
again:
    USB_STOR_PRINTF( "STATUS phase\r\n" );
    result = usb_bulk_msg( us->pusb_dev, pipein, &csw, UMASS_BBB_CSW_SIZE, &actlen, USB_CNTL_TIMEOUT * 5 );

#if 0 // debug
    int i;
    unsigned char *ttemp = ( unsigned char * )&csw;
    debugstring( "_________________________ read status\r\n" );
    for( i = 0; i < UMASS_BBB_CSW_SIZE; i++ )
    {
        debugprintf( "%02x ", ttemp[i] );
    }
    debugstring( "\r\n\n\n" );
#endif 
    /* special handling of STALL in STATUS phase */
    if( ( result < 0 ) && ( retry < 1 ) && ( us->pusb_dev->status & USB_ST_STALLED ) )
    {
        USB_STOR_PRINTF( "STATUS:stall\r\n" );
        /* clear the STALL on the endpoint */
        result = usb_stor_BBB_clear_endpt_stall( us, us->ep_in );
        if ( result >= 0 && ( retry++ < 1 ) )
            /* do a retry */
            goto again;
    }
    if ( result < 0 )
    {
        USB_STOR_PRINTF( "usb_bulk_msg error status %ld\r\n",
                         us->pusb_dev->status );
        usb_stor_BBB_reset( us );
        return USB_STOR_TRANSPORT_FAILED;
    }
#ifdef BBB_XPORT_TRACE
    ptr = ( unsigned char * )&csw;
    for ( index = 0; index < UMASS_BBB_CSW_SIZE; index++ )
        debugprintf( "ptr[%d] %#x ", index, ptr[index] );
    debugprintf( "\r\n" );
#endif

    /* misuse pipe to get the residue */
#if 0 
    pipe = swap_32( csw.dCSWDataResidue );
    if ( pipe == 0 && srb->datalen != 0 && srb->datalen - data_actlen != 0 )
        pipe = srb->datalen - data_actlen;
#endif 
    if ( CSWSIGNATURE != swap_32( csw.dCSWSignature ) )
    {
//		USB_STOR_PRINTF("!CSWSIGNATURE\r\n");
        usb_stor_BBB_reset( us );
        return USB_STOR_TRANSPORT_FAILED;
    }
    else if ( ( CBWTag - 1 ) != swap_32( csw.dCSWTag ) )
    {
//		USB_STOR_PRINTF("!Tag\r\n");
        usb_stor_BBB_reset( us );
        return USB_STOR_TRANSPORT_FAILED;
    }
    else if ( csw.bCSWStatus > CSWSTATUS_PHASE )
    {
//		USB_STOR_PRINTF(">PHASE\r\n");
        usb_stor_BBB_reset( us );
        return USB_STOR_TRANSPORT_FAILED;
    }
    else if ( csw.bCSWStatus == CSWSTATUS_PHASE )
    {
//		USB_STOR_PRINTF("=PHASE\r\n");
        usb_stor_BBB_reset( us );
        return USB_STOR_TRANSPORT_FAILED;
    }
    else if ( data_actlen > srb->datalen )
    {
//		USB_STOR_PRINTF("transferred %dB instead of %dB\r\n",
//			data_actlen, srb->datalen);
        return USB_STOR_TRANSPORT_FAILED;
    }
    else if ( csw.bCSWStatus == CSWSTATUS_FAILED )
    {
//		USB_STOR_PRINTF("FAILED\r\n");
        return USB_STOR_TRANSPORT_FAILED;
    }

    return result;
}

int usb_stor_CB_transport( ccb *srb, struct us_data *us )
{
    int result, status;
    ccb *psrb;
    ccb reqsrb;
    int retry, notready;

    psrb = &reqsrb;
    status = USB_STOR_TRANSPORT_GOOD;
    retry = 0;
    notready = 0;
    /* issue the command */
do_retry:
    result = usb_stor_CB_comdat( srb, us );
    USB_STOR_PRINTF( "command / Data returned %d, status %X\r\n", result, us->pusb_dev->status );
    /* if this is an CBI Protocol, get IRQ */
    if( us->protocol == US_PR_CBI )
    {
        status = usb_stor_CBI_get_status( srb, us );
        /* if the status is error, report it */
        if( status == USB_STOR_TRANSPORT_ERROR )
        {
            USB_STOR_PRINTF( " USB CBI Command Error\r\n" );
            return status;
        }
        srb->sense_buf[12] = ( unsigned char )( us->ip_data >> 8 );
        srb->sense_buf[13] = ( unsigned char )( us->ip_data & 0xff );
        if( !us->ip_data )
        {
            /* if the status is good, report it */
            if( status == USB_STOR_TRANSPORT_GOOD )
            {
                USB_STOR_PRINTF( " USB CBI Command Good\r\n" );
                return status;
            }
        }
    }
    /* do we have to issue an auto request? */
    /* HERE we have to check the result */
    if( ( result < 0 ) && !( us->pusb_dev->status & USB_ST_STALLED ) )
    {
        USB_STOR_PRINTF( "ERROR %X\r\n", us->pusb_dev->status );
        us->transport_reset( us );
        return USB_STOR_TRANSPORT_ERROR;
    }
    if( ( us->protocol == US_PR_CBI ) &&
        ( ( srb->cmd[0] == SCSI_REQ_SENSE ) ||
          ( srb->cmd[0] == SCSI_INQUIRY ) ) ) /* do not issue an autorequest after request sense */
    {
        USB_STOR_PRINTF( "No auto request and good\r\n" );
        return USB_STOR_TRANSPORT_GOOD;
    }
    /* issue an request_sense */
    memset( &psrb->cmd[0], 0, 12 );
    psrb->cmd[0] = SCSI_REQ_SENSE;
    psrb->cmd[1] = srb->lun << 5;
    psrb->cmd[4] = 18;
    psrb->datalen = 18;
    memcpy( sense_buff, &srb->sense_buf[0], 64 );
//	psrb->pdata=&srb->sense_buf[0];
    psrb->pdata = sense_buff;
    psrb->cmdlen = 12;
    /* issue the command */
    result = usb_stor_CB_comdat( psrb, us );
    memcpy( &srb->sense_buf[0], sense_buff, 64 );
    USB_STOR_PRINTF( "auto request returned %d\r\n", result );
    /* if this is an CBI Protocol, get IRQ */
    if( us->protocol == US_PR_CBI )
    {
        status = usb_stor_CBI_get_status( psrb, us );
    }
    if( ( result < 0 ) && !( us->pusb_dev->status & USB_ST_STALLED ) )
    {
        USB_STOR_PRINTF( " AUTO REQUEST ERROR %d\r\n", us->pusb_dev->status );
        return USB_STOR_TRANSPORT_ERROR;
    }
    USB_STOR_PRINTF( "autorequest returned 0x%02X 0x%02X 0x%02X 0x%02X\r\n", srb->sense_buf[0], srb->sense_buf[2], srb->sense_buf[12], srb->sense_buf[13] );
    /* Check the auto request result */
    if( ( srb->sense_buf[2] == 0 ) &&
        ( srb->sense_buf[12] == 0 ) &&
        ( srb->sense_buf[13] == 0 ) ) /* ok, no sense */
        return USB_STOR_TRANSPORT_GOOD;
    /* Check the auto request result */
    switch( srb->sense_buf[2] )
    {
    case 0x01: /* Recovered Error */
        return USB_STOR_TRANSPORT_GOOD;
        break;
    case 0x02: /* Not Ready */
        if( notready++ > USB_TRANSPORT_NOT_READY_RETRY )
        {
            debugprintf( "cmd 0x%02X returned 0x%02X 0x%02X 0x%02X 0x%02X (NOT READY)\r\n",
                         srb->cmd[0], srb->sense_buf[0], srb->sense_buf[2], srb->sense_buf[12], srb->sense_buf[13] );
            return USB_STOR_TRANSPORT_FAILED;
        }
        else
        {
            wait_ms( 100 );
            goto do_retry;
        }
        break;
    default:
        if( retry++ > USB_TRANSPORT_UNKNOWN_RETRY )
        {
            debugprintf( "cmd 0x%02X returned 0x%02X 0x%02X 0x%02X 0x%02X\r\n",
                         srb->cmd[0], srb->sense_buf[0], srb->sense_buf[2], srb->sense_buf[12], srb->sense_buf[13] );
            return USB_STOR_TRANSPORT_FAILED;
        }
        else
        {
            goto do_retry;
        }
        break;
    }
    return USB_STOR_TRANSPORT_FAILED;
}

static int usb_inquiry( ccb *srb, struct us_data *ss )
{
    int retry, i;
    retry = 3;
    do
    {
        memset( &srb->cmd[0], 0, 12 );
        srb->cmd[0] = SCSI_INQUIRY;
        srb->cmd[1] = srb->lun << 5;
        srb->cmd[4] = 36;
        srb->datalen = 36;
        srb->cmdlen = 12;
        i = ss->transport( srb, ss );
        USB_STOR_PRINTF( "inquiry returns %d\r\n", i );

        if( i == 0 )
            break;
    }
    while( retry-- );

    if( retry < 0 )
    {
        debugprintf( "error in inquiry\r\n" );
        return -1;
    }

    return 0;
}

// add for detecting usb memory removal
static int usb_inquiry2( ccb *srb, struct us_data *ss )
{
    memset( &srb->cmd[0], 0, 12 );
    srb->cmd[0] = SCSI_INQUIRY;
    srb->cmd[1] = srb->lun << 5;
    srb->cmd[4] = 36;
    srb->datalen = 36;
    srb->cmdlen = 12;
    if( ss->transport( srb, ss ) != USB_STOR_TRANSPORT_GOOD )
        return -1;

    return 0;
}
// add for detecting usb memory removal

static int usb_request_sense( ccb *srb, struct us_data *ss )
{
    char *ptr;
    ptr = srb->pdata;
    memset( &srb->cmd[0], 0, 12 );
    srb->cmd[0] = SCSI_REQ_SENSE;
    srb->cmd[1] = srb->lun << 5;
    srb->cmd[4] = 18;
    srb->datalen = 18;
    srb->pdata = &srb->sense_buf[0];
//	srb->pdata=&sense_buff[0];
    srb->cmdlen = 12;
    ss->transport( srb, ss );
//	memcpy(&srb->sense_buf[0], &sense_buff[0], 64);
    USB_STOR_PRINTF( "Request Sense returned %02X %02X %02X\r\n", srb->sense_buf[2], srb->sense_buf[12], srb->sense_buf[13] );
    srb->pdata = ptr;
    return 0;
}

static int usb_test_unit_ready( ccb *srb, struct us_data *ss )
{
    int retries = 10;

    do
    {
        memset( &srb->cmd[0], 0, 12 );
        srb->cmd[0] = SCSI_TST_U_RDY;
        srb->cmd[1] = srb->lun << 5;
        srb->datalen = 0;
        srb->cmdlen = 12;
        if( ss->transport( srb, ss ) == USB_STOR_TRANSPORT_GOOD )
        {
            return 0;
        }
        usb_request_sense ( srb, ss );
        // wait_ms (100);
        wait_ms ( 10 );
    }
    while( retries-- );

    return -1;
}

#if 0 
// add for detecting usb memory removal
static int usb_test_unit_ready2( ccb *srb, struct us_data *ss )
{
    int retries = 2;

    do
    {
        memset( &srb->cmd[0], 0, 12 );
        srb->cmd[0] = SCSI_TST_U_RDY;
        srb->cmd[1] = srb->lun << 5;
        srb->datalen = 0;
        srb->cmdlen = 12;
        if( ss->transport( srb, ss ) == USB_STOR_TRANSPORT_GOOD )
        {
            return 0;
        }
        usb_request_sense ( srb, ss );
        // wait_ms (100);
        wait_ms ( 1 );
    }
    while( retries-- );

    return -1;
}

#endif 

static int usb_read_capacity( ccb *srb, struct us_data *ss )
{
    int retry;
    retry = 2; /* retries */
    do
    {
        memset( &srb->cmd[0], 0, 12 );
        srb->cmd[0] = SCSI_RD_CAPAC;
        srb->cmd[1] = srb->lun << 5;
        srb->datalen = 8;
        srb->cmdlen = 12;
        if( ss->transport( srb, ss ) == USB_STOR_TRANSPORT_GOOD )
        {
            return 0;
        }
    }
    while( retry-- );

    return -1;
}

static int usb_read_10( ccb *srb, struct us_data *ss, unsigned long start, unsigned short blocks )
{
//	memset(&srb->cmd[0],0,12);
    srb->cmd[0] = SCSI_READ10;
    srb->cmd[1] = srb->lun << 5;
    srb->cmd[2] = ( ( unsigned char ) ( start >> 24 ) ) & 0xff;
    srb->cmd[3] = ( ( unsigned char ) ( start >> 16 ) ) & 0xff;
    srb->cmd[4] = ( ( unsigned char ) ( start >> 8 ) ) & 0xff;
    srb->cmd[5] = ( ( unsigned char ) ( start ) ) & 0xff;
    srb->cmd[7] = ( ( unsigned char ) ( blocks >> 8 ) ) & 0xff;
    srb->cmd[8] = ( unsigned char ) blocks & 0xff;
    srb->cmdlen = 12;
//	USB_STOR_PRINTF("read10: start %lx blocks %x\r\n",start,blocks);
    return ss->transport( srb, ss );
}

static int usb_write_10( ccb *srb, struct us_data *ss, unsigned long start, unsigned short blocks )
{
    memset( &srb->cmd[0], 0, 12 );
    srb->cmd[0] = SCSI_WRITE10;
    srb->cmd[1] = srb->lun << 5;
    srb->cmd[2] = ( ( unsigned char ) ( start >> 24 ) ) & 0xff;
    srb->cmd[3] = ( ( unsigned char ) ( start >> 16 ) ) & 0xff;
    srb->cmd[4] = ( ( unsigned char ) ( start >> 8 ) ) & 0xff;
    srb->cmd[5] = ( ( unsigned char ) ( start ) ) & 0xff;
    srb->cmd[7] = ( ( unsigned char ) ( blocks >> 8 ) ) & 0xff;
    srb->cmd[8] = ( unsigned char ) blocks & 0xff;
    srb->cmdlen = 12;
    USB_STOR_PRINTF( "write10: start %lx blocks %x\r\n", start, blocks );
    return ss->transport( srb, ss );
}

#if 0 
static int usb_write_verify( ccb *srb, struct us_data *ss, unsigned long start, unsigned short blocks )
{
    memset( &srb->cmd[0], 0, 12 );
    srb->cmd[0] = SCSI_WRT_VERIFY;
    srb->cmd[1] = srb->lun << 5;
    srb->cmd[2] = ( ( unsigned char ) ( start >> 24 ) ) & 0xff;
    srb->cmd[3] = ( ( unsigned char ) ( start >> 16 ) ) & 0xff;
    srb->cmd[4] = ( ( unsigned char ) ( start >> 8 ) ) & 0xff;
    srb->cmd[5] = ( ( unsigned char ) ( start ) ) & 0xff;
    srb->cmd[7] = ( ( unsigned char ) ( blocks >> 8 ) ) & 0xff;
    srb->cmd[8] = ( unsigned char ) blocks & 0xff;
    srb->cmdlen = 12;
    USB_STOR_PRINTF( "write&verify: start %lx blocks %x\r\n", start, blocks );
    return ss->transport( srb, ss );
}
#endif 

#define MAX_CLUSTSIZE	10240
unsigned char fat32_rw_block[MAX_CLUSTSIZE];  /* Block buffer */

#define USB_MAX_READ_BLK 200

unsigned long usb_stor_read( int device, unsigned long blknr, unsigned long blkcnt, unsigned long *buffer )
{
    unsigned long start, blks, buf_addr;
    unsigned short smallblks;
    // struct usb_device *dev;
    int retry;
    // int i;
    dcache_invalidate_way();
    ccb *srb = &usb_ccb;

    if ( blkcnt == 0 )
        return 0;

    device &= 0xff;
    /* Setup  device
     */
    USB_STOR_PRINTF( "\r\nusb_read: dev %d \r\n", device );
    // dev=NULL;
    // for(i=0;i<USB_MAX_DEVICE;i++) {
    // dev=usb_get_dev_index(i);
    // if(dev==NULL) {
    // return 0;
    // }
    // if(dev->devnum==usb_dev_desc[device].target)
    // break;
    // }

    usb_disable_asynch( 1 ); /* asynch transfer not allowed */
    srb->lun = usb_dev_desc[device].lun;
    buf_addr = ( unsigned long )buffer;
    start = blknr;
    blks = blkcnt;
    if( usb_test_unit_ready( srb, ( struct us_data * )curdev->privptr ) )
    {
        debugprintf( "Device NOT ready\r\n   Request Sense returned %02X %02X %02X\r\n",
                     srb->sense_buf[2], srb->sense_buf[12], srb->sense_buf[13] );
        return 0;
    }
    USB_STOR_PRINTF( "\r\n====> usb_read: dev %d startblk %lx, blccnt %lx buffer %lx\r\n", device, start, blks, buf_addr );
    do
    {
        retry = 2;
//		srb->pdata=(unsigned char *)buf_addr;
        if( blks > USB_MAX_READ_BLK )
        {
            smallblks = USB_MAX_READ_BLK;
        }
        else
        {
            smallblks = ( unsigned short ) blks;
        }
retry_it:
#if 0 
        if( smallblks == USB_MAX_READ_BLK )
            usb_show_progress();
#endif 
        dcache_invalidate_way();
        srb->datalen = usb_dev_desc[device].blksz * smallblks;
//		srb->pdata=(unsigned char *)fat32_rw_block;
        srb->pdata = ( unsigned char * )buf_addr;
        if( usb_read_10( srb, ( struct us_data * )curdev->privptr, start, smallblks ) )
        {
            USB_STOR_PRINTF( "Read ERROR\r\n" );
            usb_request_sense( srb, ( struct us_data * )curdev->privptr );
            if( retry-- )
                goto retry_it;
            blkcnt -= blks;
            break;
        }
//		memcpy((unsigned char *)buf_addr, fat32_rw_block, smallblks*512);
        start += smallblks;
        blks -= smallblks;
        buf_addr += srb->datalen;
    }
    while( blks != 0 );
    USB_STOR_PRINTF( "usb_read: end startblk %lx, blccnt %x buffer %lx\r\n", start, smallblks, buf_addr );
    usb_disable_asynch( 0 ); /* asynch transfer allowed */
#if 0 
    if( blkcnt >= USB_MAX_READ_BLK )
        debugprintf( "\r\n" );
#endif 

    return( blkcnt );
}

#define USB_MAX_WRITE_BLK 20
unsigned long usb_stor_write( int device, unsigned long blknr, unsigned long blkcnt, unsigned long *buffer )
{
    unsigned long start, blks, buf_addr;
    unsigned short smallblks;
    struct usb_device *dev;
    int retry, i;
    ccb *srb = &usb_ccb;

    if ( blkcnt == 0 )
        return 0;

    device &= 0xff;
    /* Setup  device
     */
//	 dev = curdev;
//	if(dev==NULL) {
//		return 0;
//	}
    USB_STOR_PRINTF( "\r\nusb_write: dev 0x%x \r\n", dev );
    USB_STOR_PRINTF( "usb_write: dev number %d \r\n", device );
    dev = NULL;
    for( i = 0; i < USB_MAX_DEVICE; i++ )
    {
        dev = usb_get_dev_index( i );
        if( dev == NULL )
        {
            return 0;
        }
        if( dev->devnum == usb_dev_desc[device].target )
            break;
    }

    usb_disable_asynch( 1 ); /* asynch transfer not allowed */
//	srb->pdata=(unsigned char *)fat32_rw_block;
    srb->lun = usb_dev_desc[device].lun;
    buf_addr = ( unsigned long )buffer;
    start = blknr;
    blks = blkcnt;
    if( usb_test_unit_ready( srb, ( struct us_data * )dev->privptr ) )
    {
        debugprintf( "Device NOT ready\r\n   Request Sense returned %02X %02X %02X\r\n",
                     srb->sense_buf[2], srb->sense_buf[12], srb->sense_buf[13] );
        return 0;
    }
    USB_STOR_PRINTF( "\r\n====> usb_write: dev %d startblk %lx, blccnt %lx buffer %lx\r\n", device, start, blks, buf_addr );
    do
    {
//		if(usb_test_unit_ready(srb,(struct us_data *)dev->privptr)) {
//			debugprintf("Device NOT ready\r\n   Request Sense returned %02X %02X %02X\r\n",
//				srb->sense_buf[2],srb->sense_buf[12],srb->sense_buf[13]);
//			return 0;
//		}
        retry = 2;
//		srb->pdata=(unsigned char *)buf_addr;
        if( blks > USB_MAX_READ_BLK )
        {
            smallblks = USB_MAX_WRITE_BLK;
        }
        else
        {
            smallblks = ( unsigned short ) blks;
        }
        memcpy( fat32_rw_block, ( unsigned char * )buf_addr, smallblks * 512 );
retry_it:
        if( smallblks == USB_MAX_WRITE_BLK )
            usb_show_progress();
        srb->datalen = usb_dev_desc[device].blksz * smallblks;
        srb->pdata = ( unsigned char * )fat32_rw_block;
//		srb->pdata=(unsigned char *)buf_addr;
        if( usb_write_10( srb, ( struct us_data * )dev->privptr, start, smallblks ) )
        {
//		if(usb_write_verify(srb,(struct us_data *)dev->privptr, start, smallblks)) {
            USB_STOR_PRINTF( "Write ERROR\r\n" );
//			debugstring(">>>Write ERROR\r\n");
            usb_request_sense( srb, ( struct us_data * )dev->privptr );
            if( retry-- )
                goto retry_it;
            blkcnt -= blks;
            break;
        }
//		do {
//			usb_request_sense(srb,(struct us_data *)dev->privptr);
//			debugprintf("srb->sense_buf[2] = %d\r\n", srb->sense_buf[2]);
//
//		} while(srb->sense_buf[2] !=0);
        start += smallblks;
        blks -= smallblks;
        buf_addr += srb->datalen;
    }
    while( blks != 0 );
    USB_STOR_PRINTF( "usb_write: end startblk %lx, blccnt %x buffer %lx\r\n", start, smallblks, buf_addr );
    usb_disable_asynch( 0 ); /* asynch transfer allowed */
//	if(blkcnt>=USB_MAX_WRITE_BLK)
//		debugprintf("\r\n");
    return( blkcnt );
}

/* Probe to see if a new device is actually a Storage device */
int usb_storage_probe( struct usb_device *dev, unsigned int ifnum, struct us_data *ss )
{
    struct usb_interface_descriptor *iface;
    int i;
    unsigned int flags = 0;

    int protocol = 0;
    int subclass = 0;


    memset( ss, 0, sizeof( struct us_data ) );

    /* let's examine the device now */
    iface = &dev->config.if_desc[ifnum];
#if 0 // debug
    unsigned char temp[100];
    debugprintf( "_______________ interface descriptor\r\n" );
    memcpy( temp, &dev->config.if_desc[ifnum], 9 );
    for( i = 0; i < 9; i++ )
    {
        if( !( i % 8 ) ) debugprintf( "\r\n" );
        debugprintf( "%02x ", temp[i] );
    }
    debugprintf( "\r\n\n" );
#endif 
#if 0
    /* this is the place to patch some storage devices */
    USB_STOR_PRINTF( "iVendor %X iProduct %X\r\n", dev->descriptor.idVendor, dev->descriptor.idProduct );
    if ( ( dev->descriptor.idVendor ) == 0x066b && ( dev->descriptor.idProduct ) == 0x0103 )
    {
        USB_STOR_PRINTF( "patched for E-USB\r\n" );
        protocol = US_PR_CB;
        subclass = US_SC_UFI;	    /* an assumption */
    }
#endif

#if 0 // debug
    debugprintf( "_______________ device descriptor\r\n" );
    memcpy( temp, &dev->descriptor, 0x12 );
    for( i = 0; i < 0x12; i++ )
    {
        if( !( i % 8 ) ) debugprintf( "\r\n" );
        debugprintf( "%02x ", temp[i] );
    }
    debugprintf( "\r\n\n" );
#endif 
    if ( dev->descriptor.bDeviceClass != 0 ||
         iface->bInterfaceClass != USB_CLASS_MASS_STORAGE ||
         iface->bInterfaceSubClass < US_SC_MIN ||
         iface->bInterfaceSubClass > US_SC_MAX )
    {
        /* if it's not a mass storage, we go no further */
        return 0;
    }

    /* At this point, we know we've got a live one */
    USB_STOR_PRINTF( "\r\n\r\nUSB Mass Storage device detected\r\n" );

    /* Initialize the us_data structure with some useful info */
    ss->flags = flags;
    ss->ifnum = ifnum;
    ss->pusb_dev = dev;
    ss->attention_done = 0;

    /* If the device has subclass and protocol, then use that.  Otherwise,
     * take data from the specific interface.
     */
    if ( subclass )
    {
        ss->subclass = subclass;
        ss->protocol = protocol;
    }
    else
    {
        ss->subclass = iface->bInterfaceSubClass;
        ss->protocol = iface->bInterfaceProtocol;
    }

    /* set the handler pointers based on the protocol */
    USB_STOR_PRINTF( "Transport: " );
    switch ( ss->protocol )
    {
    case US_PR_CB:
        USB_STOR_PRINTF( "Control/Bulk\r\n" );
        ss->transport = usb_stor_CB_transport;
        ss->transport_reset = usb_stor_CB_reset;
        break;

    case US_PR_CBI:
        USB_STOR_PRINTF( "Control/Bulk/Interrupt\r\n" );
        ss->transport = usb_stor_CB_transport;
        ss->transport_reset = usb_stor_CB_reset;
        break;
    case US_PR_BULK:
        USB_STOR_PRINTF( "Bulk/Bulk/Bulk\r\n" );
        ss->transport = usb_stor_BBB_transport;
        ss->transport_reset = usb_stor_BBB_reset;
        break;
    default:
        debugprintf( "USB Storage Transport unknown / not yet implemented\r\n" );
        return 0;
        break;
    }

    /*
     * We are expecting a minimum of 2 endpoints - in and out (bulk).
     * An optional interrupt is OK (necessary for CBI protocol).
     * We will ignore any others.
     */
    for ( i = 0; i < iface->bNumEndpoints; i++ )
    {
        /* is it an BULK endpoint? */
        if ( ( iface->ep_desc[i].bmAttributes &  USB_ENDPOINT_XFERTYPE_MASK )
             == USB_ENDPOINT_XFER_BULK )
        {
            if ( iface->ep_desc[i].bEndpointAddress & USB_DIR_IN )
                ss->ep_in = iface->ep_desc[i].bEndpointAddress &
                            USB_ENDPOINT_NUMBER_MASK;
            else
                ss->ep_out = iface->ep_desc[i].bEndpointAddress &
                             USB_ENDPOINT_NUMBER_MASK;
        }

        /* is it an interrupt endpoint? */
        if ( ( iface->ep_desc[i].bmAttributes & USB_ENDPOINT_XFERTYPE_MASK )
             == USB_ENDPOINT_XFER_INT )
        {
            ss->ep_int = iface->ep_desc[i].bEndpointAddress &
                         USB_ENDPOINT_NUMBER_MASK;
            ss->irqinterval = iface->ep_desc[i].bInterval;
        }
    }
    USB_STOR_PRINTF( "Endpoints In %d Out %d Int %d\r\n",
                     ss->ep_in, ss->ep_out, ss->ep_int );

    /* Do some basic sanity checks, and bail if we find a problem */
    if ( usb_set_interface( dev, iface->bInterfaceNumber, 0 ) ||
         !ss->ep_in || !ss->ep_out ||
         ( ss->protocol == US_PR_CBI && ss->ep_int == 0 ) )
    {
        USB_STOR_PRINTF( "Problems with device\r\n" );
        //return 0;
    }
    /* set class specific stuff */
    /* We only handle certain protocols.  Currently, these are
     * the only ones.
     * The SFF8070 accepts the requests used in u-boot
     */
    if ( ss->subclass != US_SC_UFI && ss->subclass != US_SC_SCSI &&
         ss->subclass != US_SC_8070 )
    {
        debugprintf( "Sorry, protocol %d not yet supported.\r\n", ss->subclass );
        return 0;
    }
    if( ss->ep_int ) /* we had found an interrupt endpoint, prepare irq pipe */
    {
        /* set up the IRQ pipe and handler */

        ss->irqinterval = ( ss->irqinterval > 0 ) ? ss->irqinterval : 255;
        ss->irqpipe = usb_rcvintpipe( ss->pusb_dev, ss->ep_int );
        ss->irqmaxp = usb_maxpacket( dev, ss->irqpipe );
        dev->irq_handle = usb_stor_irq;
    }
    dev->privptr = ( void * )ss;
    return 1;
}

static unsigned long cap[2];
int usb_stor_get_info( struct usb_device *dev, struct us_data *ss, block_dev_desc_t *dev_desc )
{
    unsigned char perq, modi;
    //unsigned long cap[2];
    unsigned long *capacity, *blksz;
    ccb *pccb = &usb_ccb;

    ss->transport_reset( ss );
    pccb->pdata = usb_stor_buf;

    dev_desc->target = dev->devnum;
    pccb->lun = dev_desc->lun;
    USB_STOR_PRINTF( " address %d\r\n", dev_desc->target );

    if( usb_inquiry( pccb, ss ) )
        return -1;
#if 0 // debug
    int i;
    debugprintf( "\r\n" );
    for( i = 0; i < 36; i++ )
    {
        debugprintf( "%02x ", usb_stor_buf[i] );
    }
    debugprintf( "\r\n\n\n" );
#endif 
    perq = usb_stor_buf[0];
    modi = usb_stor_buf[1];
    if( ( perq & 0x1f ) == 0x1f )
    {
        return 0; /* skip unknown devices */
    }
    if( ( modi & 0x80 ) == 0x80 ) /* drive is removable */
    {
        dev_desc->removable = 1;
    }
    memcpy( &dev_desc->vendor[0], &usb_stor_buf[8], 8 );
    memcpy( &dev_desc->product[0], &usb_stor_buf[16], 16 );
    memcpy( &dev_desc->revision[0], &usb_stor_buf[32], 4 );
    dev_desc->vendor[8] = 0;
    dev_desc->product[16] = 0;
    dev_desc->revision[4] = 0;
    USB_STOR_PRINTF( "ISO Vers %X, Response Data %X\r\n", usb_stor_buf[2], usb_stor_buf[3] );
    if( usb_test_unit_ready( pccb, ss ) )
    {
        debugprintf( "Device NOT ready\r\n   Request Sense returned %02X %02X %02X\r\n", pccb->sense_buf[2], pccb->sense_buf[12], pccb->sense_buf[13] );
        if( dev_desc->removable == 1 )
        {
            dev_desc->type = perq;
            return 1;
        }
        else
            return 0;
    }

    pccb->pdata = ( unsigned char * )&cap[0];
    memset( pccb->pdata, 0, 8 );
    if( usb_read_capacity( pccb, ss ) != 0 )
    {
        debugprintf( "READ_CAP ERROR\r\n" );
        cap[0] = 2880;
        cap[1] = 0x200;
    }
    USB_STOR_PRINTF( "Read Capacity returns: 0x%lx, 0x%lx\r\n", cap[0], cap[1] );
#if 0
    if( cap[0] > ( 0x200000 * 10 ) ) /* greater than 10 GByte */
        cap[0] >>= 16;
#endif
#if 1 //#ifdef LITTLEENDIAN
    cap[0] = ( ( unsigned long )(
                   ( ( ( unsigned long )( cap[0] ) & ( unsigned long )0x000000ffUL ) << 24 ) |
                   ( ( ( unsigned long )( cap[0] ) & ( unsigned long )0x0000ff00UL ) <<  8 ) |
                   ( ( ( unsigned long )( cap[0] ) & ( unsigned long )0x00ff0000UL ) >>  8 ) |
                   ( ( ( unsigned long )( cap[0] ) & ( unsigned long )0xff000000UL ) >> 24 ) ) );
    cap[1] = ( ( unsigned long )(
                   ( ( ( unsigned long )( cap[1] ) & ( unsigned long )0x000000ffUL ) << 24 ) |
                   ( ( ( unsigned long )( cap[1] ) & ( unsigned long )0x0000ff00UL ) <<  8 ) |
                   ( ( ( unsigned long )( cap[1] ) & ( unsigned long )0x00ff0000UL ) >>  8 ) |
                   ( ( ( unsigned long )( cap[1] ) & ( unsigned long )0xff000000UL ) >> 24 ) ) );
#endif
    /* this assumes bigendian! */
    cap[0] += 1;
    capacity = &cap[0];
    blksz = &cap[1];
    USB_STOR_PRINTF( "Capacity = 0x%lx, blocksz = 0x%lx\r\n", *capacity, *blksz );
    dev_desc->lba = *capacity;
    dev_desc->blksz = *blksz;
    dev_desc->type = perq;
    USB_STOR_PRINTF( " address %d\n", dev_desc->target );
    USB_STOR_PRINTF( "partype: %d\n", dev_desc->part_type );

    init_part( dev_desc );

    USB_STOR_PRINTF( "partype: %d\n", dev_desc->part_type );
    return 1;
}


int usb_stor_ready_check( struct usb_device *dev, struct us_data *ss, block_dev_desc_t *dev_desc )
{
//	unsigned char perq,modi;
//  unsigned long cap[2];
//	unsigned long *capacity,*blksz;
    ccb *pccb = &usb_ccb;

//	ss->transport_reset(ss); 
    pccb->pdata = usb_stor_buf;

    dev_desc->target = dev->devnum;
    pccb->lun = dev_desc->lun;
//	USB_STOR_PRINTF(" address %d\r\n",dev_desc->target);

    if( usb_inquiry2( pccb, ss ) )
    {
        USB_STOR_PRINTF( "usb_inquiry ERROR!!!!\r\n" );
        return 0;
    }
	
#if 0 // debug
    perq = usb_stor_buf[0];
    modi = usb_stor_buf[1];
    if( ( perq & 0x1f ) == 0x1f )
    {
        return 0; /* skip unknown devices */
    }
    if( ( modi & 0x80 ) == 0x80 ) /* drive is removable */
    {
        dev_desc->removable = 1;
//		debugprintf("drive is removable\r\n");
    }
//	else {
//		debugprintf("drive is removable\r\n");
//	}
//	memcpy(&dev_desc->vendor[0], &usb_stor_buf[8], 8);
//	memcpy(&dev_desc->product[0], &usb_stor_buf[16], 16);
//	memcpy(&dev_desc->revision[0], &usb_stor_buf[32], 4);
//	dev_desc->vendor[8]=0;
//	dev_desc->product[16]=0;
//	dev_desc->revision[4]=0;
//	USB_STOR_PRINTF("ISO Vers %X, Response Data %X\r\n",usb_stor_buf[2],usb_stor_buf[3]);
    if( usb_test_unit_ready2( pccb, ss ) )
    {
        debugprintf( "Device NOT ready\r\n   Request Sense returned %02X %02X %02X\r\n", pccb->sense_buf[2], pccb->sense_buf[12], pccb->sense_buf[13] );
        if( dev_desc->removable == 1 )
        {
            dev_desc->type = perq;
        }
        else
        {            
//			return 0;
        }
        return 0;
    }
#endif 
    return 1;
}

// add
int get_usb_storage_removed( void )
{
    int i;

    if( get_dev_index() != 0 )
    {
        for( i = 0; i < USB_MAX_STOR_DEV; i++ )
        {
            if( usb_stor_ready_check( curdev, &usb_stor[i], &usb_dev_desc[i] ) )
            {
                USB_STOR_PRINTF( "usb storage is running!!\r\n" );
                return 0;
            } /* if get info ok */
            else
            {
                USB_STOR_PRINTF( "usb storage is removed!!\r\n" );

                cur_dev = NULL;
                if( !usbhostinit() ) debugprintf( "USB Host reInit Fail~~\r\n" );
                return 1;
            }
        }
    }
    debugprintf( "usb host isn't initialized!\r\n" );
    return 1;
}
// add

//extern block_dev_desc_t * usb_stor_get_dev(int dev);

int current_dev_set( void )
{
    // int i;
    block_dev_desc_t *dev_desc = NULL;

    dev_desc = usb_stor_get_dev( 0 );

    if ( ( !dev_desc->block_read ) || ( !dev_desc->block_write ) )
    {
        cur_dev = NULL;
        return -1;
    }
    if( usb_max_devs == 0 )
    {
        cur_dev = NULL;
        return -1;
    }
    cur_dev = dev_desc;

    // curdev = NULL;
    // for(i=0;i<USB_MAX_DEVICE;i++) {
    // curdev=usb_get_dev_index(i);
    // if(curdev==NULL) {
    // return 0;
    // }
    // if(curdev->devnum==usb_dev_desc[cur_dev->dev].target)
    // break;
    // }

    return 0;
}

int USB_GetSectorCount()
{
    return usb_dev_desc[cur_dev->dev].lba;
}

int USB_GetSectorSize()
{
    return usb_dev_desc[cur_dev->dev].blksz;
}

int read_disk ( __u32 startblock, __u32 getsize, __u8 * bufptr )
{
    if ( cur_dev == NULL )
        return -1;
    if ( cur_dev->block_read ) // --> <usb_stor_read>
    {
        return cur_dev->block_read ( cur_dev->dev, startblock, getsize, ( unsigned long * )bufptr );
    }
    return -1;
}

int write_disk ( __u32 startblock, __u32 getsize, __u8 * bufptr )
{
    if ( cur_dev == NULL )
        return -1;
    if ( cur_dev->block_write ) // --> <usb_stor_write>
    {
        return cur_dev->block_write ( cur_dev->dev, startblock, getsize, ( unsigned long * )bufptr );
    }
    return -1;
}

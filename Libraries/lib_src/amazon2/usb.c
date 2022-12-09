/*****************************************************************************
*
* Copyright (C) 2014      Advanced Digital Chips, Inc. All Rights Reserved.
*						http://www.adc.co.kr
*
* THIS SOFTWARE IS PROVIDED BY ADCHIPS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ADCHIPS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE
*
*****************************************************************************/

#include "sdk.h"

//#define USB_DEBUG
#ifdef USB_DEBUG
#define usb_debugstring(A)	debugstring(A)
#define usb_debugprintf	DEBUGPRINTF
#else
#define usb_debugstring(A)
#define usb_debugprintf(...)
#endif


//----------------------------------
// FUNCTION ADDRESS
//----------------------------------
#define ADDR_UPDATA         0x80

//----------------------------------
// ENDPOINT Register
//----------------------------------
#define EP0                 0x00
#define EP1                 0x01
#define EP2                 0x02
#define EP3                 0x03
#define EP4                 0x04

//----------------------------------
// Power management register
//----------------------------------
#define PW_ENABLE_SUSPEND    0x01
#define PW_SUSPEND_MODE      0x02
#define PW_UC_RESUME         0x04
#define PW_USB_RESET         0x08
#define PW_ISO_UPDATE        0x80

//----------------------------------
// USB Interrupt Enable Register
//----------------------------------
#define SUSPEND_INTERRUPT_ENABLE    0x01
#define USB_RESET_INTERRUPT_ENABLE  0x04


//----------------------------------
// ENDPOINT 0 CSR Register
//----------------------------------
#define EP0_OUT_PKT_RDY             0x01
#define EP0_IN_PKT_RDY              0x02
#define EP0_SENT_STALL              0x04
#define EP0_DATA_END                0x08
#define EP0_SETUP_END               0x10
#define EP0_SEND_STALL              0x20
#define EP0_SERVICED_OUT_PKT_RDY    0x40
#define EP0_SERVICED_SETUP_END      0x80

#define EP0_CLR_SENT_STALL          0xFB

//----------------------------------
// IN CSR1 Register
//----------------------------------
#define IN_CSR_IN_PKT_RDY           0x01
#define IN_CSR_FIFO_NOT_EMPTY       0x02
#define IN_CSR_UNDER_RUN            0x04
#define IN_CSR_FIFO_FLUSH           0x08
#define IN_CSR_SEND_STALL           0x10
#define IN_CSR_SENT_STALL           0x20
#define IN_CSR_CLR_DATA_TOGGLE      0x40

#define IN_CSR_DMA_MODE             0x10
#define IN_CSR_MODE_IN              0x20
#define IN_CSR_ISO                  0x40
#define IN_CSR_AUTO_SET             0x80

//----------------------------------
// OUT CSR1 Register
//----------------------------------
#define OUT_CSR_OUT_PKT_RDY         0x01
#define OUT_CSR_FIFO_FULL           0x02
#define OUT_CSR_OVER_RUN            0x04
#define OUT_CSR_DATA_ERROR          0x08
#define OUT_CSR_FIFO_FLUSH          0x10
#define OUT_CSR_SEND_STALL          0x20
#define OUT_CSR_SENT_STALL          0x40
#define OUT_CSR_CLR_DATA_TOGGLE     0x80

#define OUT_CSR_ISO                 0x40
#define OUT_CSR_AUTO_CLR            0x80


//----------------------------------
// Standard Chapter 9 definition
//----------------------------------
#define STD_REQUEST			0x00
#define CLASS_REQUEST		0x01
#define VENDOR_REQUEST		0x02
#define RECIPIENT_DEV		0x00
#define RECIPIENT_IFC		0x01
#define RECIPIENT_ENP		0x02

#define ENDP_STALL          0x00
#define REMOTE_WAKEUP       0x01

#define GET_STATUS      	0x00
#define CLEAR_FEATURE   	0x01
#define SET_FEATURE    		0x03
#define SET_ADDRESS    		0x05
#define GET_DESCRIPTOR 		0x06
#define SET_DESCRIPTOR 		0x07
#define GET_CONFIG     		0x08
#define SET_CONFIG    		0x09
#define GET_INTERFACE  		0x0A
#define SET_INTERFACE  		0x0B
#define SYNCH_FRAME     	0x0C


#define GET_MS_DESCRIPTOR  0xc0 //for WINUSB

#define DEVICE          	0x01
#define CONFIGURATION   	0x02
#define STRING          	0x03
#define INTERFACE       	0x04
#define ENDPOINT        	0x05

#define STDCLASS        	0x00
#define HIDCLASS        	0x03
#define HUBCLASS	 		0x09

//----------------------------------
// Class Descriptor for HID
//----------------------------------
#define GET_REPORT      	0x01
#define GET_IDLE        	0x02
#define GET_PROTOCOL    	0x03

#define SET_REPORT      	0x09
#define SET_IDLE        	0x0A
#define SET_PROTOCOL    	0x0B
#define HID_DEV         	0x21
#define HID_REPORT      	0x22
#define HID_PHYSICAL    	0x23

#define INPUT        		0x01
#define OUPUT     	    	0x02
#define FEATURE    	   		0x03

#define NONE	        	0x00
#define KEYBOARD        	0x01
#define MOUSE	        	0x02

//----------------------------------
// EP0 States
//----------------------------------
#define EP0_IDLE            0x00
#define EP0_IN_DATA_PHASE   0x01
#define EP0_OUT_DATA_PHASE  0x02
#define EP0_END_XFER        0x03


#define BULKONLY_GET_MAX_LUN 254
#define BULKONLY_RESET       255

#define LOGIC_UNIT_NUM         0       // Modify this to support Multi LUN


#define peekc(a) (*((volatile unsigned char  *) (a)))
#define peeks(a) (*((volatile unsigned short *) (a)))
#define peekl(a) (*((volatile unsigned long  *) (a)))

#define pokec(a, d) ( *((volatile unsigned char  *)(a))=(unsigned char)(d) )
#define pokes(a, d) ( *((volatile unsigned short *)(a))=(unsigned short)(d) )
#define pokel(a, d) ( *((volatile unsigned long  *)(a))=(unsigned long)(d) )


void usb_init();
void ep0_init();
void ep1_init();
void ep2_init();

typedef struct
{
    BYTE bmRequest;					// SETUP Token Protocol
    BYTE bRequest;
    WORD wValue;
    WORD wIndex;
    WORD wLength;
} 	SetupPKG, *pSetupPKG;

SetupPKG 	dReq;					// Setup token struct
WORD		len_req;				// length of data for EP0

BYTE        ep0_state = 0x00;             // EP0 States
BYTE        want_more_data ;

BYTE	Slave_USBaddr;			// USB device address
BYTE	Slave_ConfigVal;		// Device configuration value
BYTE	Slave_RemoteWU;			// Device remote wakeup stats
BYTE	Slave_inEPstall;		// EP0 ~ EP7's IN stall status
BYTE	Slave_outEPstall;		// EP0 ~ EP7's OUT stall status
BYTE	Slave_EPstall;		    // EP0 ~ EP5's stall status

BOOL 		BUS_POWERED;			// Bus powered device
BYTE      * dec_rom_addr ;          // Descriptor Address
BYTE xmit_len ;
BYTE	Slave_IfcAlt[MAXIFCNUM];// 8 interface(Ep0~7) contain alternate setting value


void BufRead( DWORD addr, BYTE *s, WORD c )
{
    while( c-- )
    {
        *s = peekc( addr );
        s++;
    }
}

void BufWrite( unsigned long addr, unsigned char *s, unsigned short c )
{
    while ( c-- )
    {
        pokec( ( volatile BYTE* )addr, *s++ );
    }
}

void ep2_init()
{
    BYTE status2;
    // Set  mode IN
    pokec( __USBIND_REG, EP2 );
    status2 = peekc( __USBIC2_REG );
    pokec( __USBIC2_REG, status2 | IN_CSR_MODE_IN ) ;
    // Set Auto_set
    //pokec(__USBIC2_REG, status2 | IN_CSR_AUTO_SET ) ;
    pokec( __USBIND_REG, EP2 );
    //pokec(__USBIE0M_REG, EP2_LEN/8); // Maximum Buffer Size : 64bytes
    pokec( __USBMAXP_REG, 0x08 ); // Maximum Buffer Size : 64bytes
}

void ep1_init()
{
    BYTE status2;
    // Set  mode out
    pokec( __USBIND_REG, EP1 );
    status2 = peekc( __USBIC2_REG );
    pokec( __USBIC2_REG, status2 & 0xDF ) ; // OUT MODE
    // Set Auto_clr
    pokec( __USBIND_REG, EP1 );
    status2 = peekc( __USBOC2_REG );
    pokec( __USBOC2_REG, status2 | OUT_CSR_AUTO_CLR ) ;
    pokec( __USBIND_REG, EP1 );
    pokec( __USBMAXP_REG, 0x08 ); // Maximum Buffer Size : 64bytes
}
void ep0_init( void )
{
    pokec( __USBUIE_REG, USB_RESET_INTERRUPT_ENABLE | SUSPEND_INTERRUPT_ENABLE );
    pokec( __USBIND_REG,  EP0 );
    pokec( __USBMAXP_REG, 0x02 ); // Maximum Buffer Size : 16bytes
}

void usb_reset( void )
{
    usb_debugstring( "\r\n >>> Received Reset Signaling <<<" );
    //pokec(__USBUI_REG, USB_RESET_IRQ);	    // clear USB interrupt
    usb_init();
    ep0_init();
    ep1_init();
    ep2_init();
}

void usb_resume( void )
{
    usb_debugstring( "\r\n >>> USB Resume Interrupt Routines <<<" );
    //pokec(__USBUI_REG, USB_RESUME_IRQ);	    // clear USB interrupt
}

void usb_suspend( void )
{
//	BYTE status ;
    usb_debugstring( "\r\n >>> Received Suspend Signaling <<<" );
    usb_debugstring( "\r\n     POWER MANAGEMENT Register : " );
    //status = peekc(__USBPM_REG);
    pokec( __USBPM_REG,  PW_ENABLE_SUSPEND ); // Enable suspend mode
    // Generates a resume signaling
    //pokec(__USBPM_REG,  status | PW_UC_RESUME );
    //pokec(__USBUI_REG, USB_SUSPEND_IRQ);	// clear USB interrupt
}


void set_ep_out_stall( BYTE ep_num )
{
    BYTE status ;
    usb_debugprintf( "\r\n *** ISSUING STALL EP(%d)-OUT ***", ep_num );
    pokec( __USBIND_REG, ep_num );  //
    status = peekc( __USBOC1_REG ) ; // OUT-CSR1 register
    pokec( __USBOC1_REG, status | OUT_CSR_SEND_STALL );
}

void clr_ep_in_stall( BYTE ep_num )
{
    BYTE status;
    //vputs("\r\n *** CLEARING EP(" ); vputb(ep_num); vputs( ")-IN STALL ***" );
    usb_debugprintf( "\r\n *** CLEARING EP(%x)-IN STALL ***" , ep_num );
    pokec( __USBIND_REG, ep_num );        //
    status = peekc( __USBIC1_REG ) ;      // IN-CSR1 register
    pokec( __USBIC1_REG, status & 0xCE ) ; // Clear IN_PKT_RDY, SEND_STALL, SENT_STALL
}

void clr_ep_out_stall( BYTE ep_num )
{
    BYTE status;
    //vputs("\r\n *** CLEARING EP(" ); vputb(ep_num); vputs( ")-OUT STALL ***" );
    usb_debugprintf( "\r\n *** CLEARING EP(%x)-OUT STALL ***", ep_num );
    pokec( __USBIND_REG, ep_num );        //
    status = peekc( __USBOC1_REG ) ;      // OUT-CSR1 register
    pokec( __USBOC1_REG, status &  0xBF ); // Clear SEND_STALL
}

void usb_init()
{
    ep0_state = 0x00 ;
    //----------------------------
    // USB-Specific
    //----------------------------
    //BUS_POWERED = 1;						// define as a bus powered device
    BUS_POWERED = 0;						// define as a bus powered device
    Slave_USBaddr = 0;						// set to default USB address zero
    Slave_ConfigVal = 0;					// default device config value
    Slave_RemoteWU = 0;						// device remote wakeup support
    Slave_EPstall = 0;					    // EP0 ~ EP5's Stall
    want_more_data = 0 ;  // 2005.4.11
    //pokec(__USBPM_REG,  PW_ENABLE_SUSPEND);
    //
    pokec( __USBUIE_REG, USB_RESET_INTERRUPT_ENABLE | SUSPEND_INTERRUPT_ENABLE );
    pokec( __USBEIE_REG, 0x1F );
    ep0_init();
    ep1_init();
    ep2_init();
}
static BYTE* usb_cfg_description;
static BYTE* usb_dev_description;
static BYTE* usb_string_description=NULL;
void usb_set_description( BYTE* cfg, BYTE* dev )
{
    usb_cfg_description = cfg;
    usb_dev_description = dev;
}
void WriteEP0( )
{
    BYTE status ;
    BYTE len_xfr ;
    //static BYTE xmit_len = 0 ;
    pokec( __USBIND_REG, EP0 );    // Endpoint 0
    status = peekc( __USBE0C_REG ) ; // Endpoint 0 CSR 1 register
    if ( status & EP0_SENT_STALL )
    {
        usb_debugstring( "\r\n *** USB CORE SENT STALL HANDSHAKE ***" );
        pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
        pokec( __USBE0C_REG, status & EP0_CLR_SENT_STALL ) ; // Endpoint 0 CSR1 register
        ep0_state = EP0_IDLE ;
    }
    else if ( status & EP0_SETUP_END )
    {
        usb_debugstring( "\r\n *** Received SETUP END ***" );
        pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_SERVICED_SETUP_END ) ; // Endpoint 0 CSR1 register
        ep0_state = EP0_IDLE ;
    }
    // Previous data is sent sussefully by USB
    else if ( !( status & EP0_IN_PKT_RDY ) )
    {
        if ( len_req > EP0_LEN )
            len_xfr = EP0_LEN ;
        else
            len_xfr = len_req ;
        //usb_debugprintf("\r\n len_xfr : %x:%x \r\n", len_req, len_xfr);
        BufWrite( __USBEP0D_REG, ( BYTE* )( dec_rom_addr + xmit_len ), len_xfr );
        len_req -= len_xfr ;
        xmit_len += len_xfr ;
        //usb_debugprintf("\r\n XMIT2 : %x \r\n", xmit_len);
        if ( len_req == 0 )
        {
            if ( want_more_data )
            {
                //ep0_state = (status & EP0_SETUP_END ) ? EP0_IN_DATA_PHASE : EP0_IDLE ;
                ep0_state = EP0_IDLE ;
            }
            // DATA END
            else
            {
                pokec( __USBIND_REG, EP0 );            // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_DATA_END | EP0_IN_PKT_RDY ) ; // Endpoint 0 CSR register
                ep0_state = EP0_IDLE ;
                xmit_len = 0 ;
            }
        }
        // Continues transfer
        else
        {
            pokec( __USBIND_REG, EP0 );                     // Endpoint 0
            pokec( __USBE0C_REG, status | EP0_IN_PKT_RDY ) ; // Endpoint 0 CSR register
        }
    }
}


int read_usb_r( BYTE *xmit_data, int len )
{
    int k;
    int xfer_len;
    int xfer_len_low, xfer_len_high ;
    BYTE status, status2 ;
    pokec( __USBIND_REG, EP1 );
    status = peekc( __USBOC1_REG );
    if ( status & OUT_CSR_SENT_STALL )
    {
        usb_debugstring( "\r\n *** USB CORE ISSUDED STALL HANDSHAKE ***" );
        pokec( __USBIND_REG, EP1 );
        //pokec(__USBOC1_REG, status & 0xDF);  // Clear SENT_STALL
        pokec( __USBOC1_REG, status | 0x10 ); // Clear SENT_STALL
        //return -1;
    }
    if ( status & OUT_CSR_OUT_PKT_RDY )
    {
        if ( !( status & OUT_CSR_DATA_ERROR ) )
        {
            pokec( __USBIND_REG, EP1 );
            xfer_len_low  = peekc( __USBLBOWC_REG );
            xfer_len_high = peekc( __USBHBOWC_REG );
            xfer_len = xfer_len_low ;
            if( xfer_len == 64 )
            {
                for( k = 0; k < 64; )
                {
                    xmit_data[k++] = *( volatile BYTE* )__USBEP1D_REG;
                    xmit_data[k++] = *( volatile BYTE* )__USBEP1D_REG;
                    xmit_data[k++] = *( volatile BYTE* )__USBEP1D_REG;
                    xmit_data[k++] = *( volatile BYTE* )__USBEP1D_REG;
                }
            }
            else
            {
                for( k = 0; k < xfer_len; k++ )
                {
                    xmit_data[k] = *( volatile BYTE* )__USBEP1D_REG;
                }
            }
            pokec( __USBIND_REG, EP1 );
            status2 = peekc( __USBOC2_REG );
            if ( !( status2 & OUT_CSR_AUTO_CLR ) )
            {
                usb_debugstring( "\r\n read END " );
                pokec( __USBIND_REG, EP1 );
                pokec( __USBOC1_REG, status | OUT_CSR_OUT_PKT_RDY ); // Clear OUT_CSR_OUT_PKT_RDY
            }
            return xfer_len;
        }
        else
        {
            usb_debugstring( "\r\n *** FLUSHING EP1-OUT FIFO ***" );
            pokec( __USBIND_REG, EP1 );
            status = peekc( __USBOC1_REG );
            pokec( __USBOC1_REG, status | OUT_CSR_FIFO_FLUSH ); // Clear FIFO_FULL
            return 0;
        }
    }
    usb_debugstring( "\r\n READ USB Invalid\r\n" );
    return 0;
}

int read_usb( BYTE *xmit_data )
{
    int rev;
    while( 1 )
    {
        rev = read_usb_r( xmit_data, 64 );
        if( rev > 0 )
            break;
    }
    return rev;
}



int write_usb_r( BYTE *data, int len )
{
    int k;
    WORD xfer_len ;
    BYTE status;
    //BYTE status2 ;
    usb_debugstring( "write_usb()\r\n" );
    if ( len > 64 )
        xfer_len = 64;
    else
        xfer_len = len;
    pokec( __USBIND_REG, EP2 );
    status = peekc( __USBIC1_REG );
    if ( status & IN_CSR_SENT_STALL )
    {
        usb_debugstring( "\r\n *** SENT STALL CONDITION DETECTED ***" );
        pokec( __USBIC1_REG, status & 0xDF ); // Clear SENT_STALL
    }
//    MYDEBUGPRINTF("status    : 0x%x",status);
    if ( ! ( status & IN_CSR_IN_PKT_RDY ) )
    {
        usb_debugstring( "\r\n >> Write USB << \r\n" );
        if( xfer_len == 64 )
        {
            for( k = 0; k < 64; )
            {
                *( volatile BYTE* )__USBEP2D_REG = data[k];
                *( volatile BYTE* )__USBEP2D_REG = data[k + 1];
                *( volatile BYTE* )__USBEP2D_REG = data[k + 2];
                *( volatile BYTE* )__USBEP2D_REG = data[k + 3];
                *( volatile BYTE* )__USBEP2D_REG = data[k + 4];
                *( volatile BYTE* )__USBEP2D_REG = data[k + 5];
                *( volatile BYTE* )__USBEP2D_REG = data[k + 6];
                *( volatile BYTE* )__USBEP2D_REG = data[k + 7];
                k += 8;
            }
            //status2 = peekc(__USBIC2_REG);
            //if ( !( status2 & IN_CSR_AUTO_SET )  )
            pokec( __USBIC1_REG, status | IN_CSR_IN_PKT_RDY );
        }
        else
        {
            for( k = 0; k < xfer_len; k++ )
                *( volatile BYTE* )__USBEP2D_REG = data[k];
            //status2 = peekc(__USBIC2_REG);
            pokec( __USBIC1_REG, status | IN_CSR_IN_PKT_RDY );
        }
    }
    else
    {
        usb_debugstring( "did not write\r\n" );
        return 0;
    }
    return 1;
}

int write_usb( BYTE *data, int len )
{
    if( len == 0 )
    {
        while( !( write_usb_r( data, 0 ) ) );
    }
    else
    {
        int i;
        for( i = 0; i < len ; )
        {
            if( len - i > 64 )
            {
                while( !write_usb_r( data + i, 64 ) );
                i += 64;
            }
            else
            {
                while( !( write_usb_r( data + i, len - i ) ) );
                i += ( len - i );
            }
        }
    }
    return 1;
}

void ProcessStdDevReq( BYTE status )
{
    BYTE ep_num ;
    usb_debugstring( "\r\nProcessStdDevReq" );
    switch ( dReq.bRequest )                            // Parse bRequest
    {
    case SYNCH_FRAME :
        usb_debugstring( "\r\nSYNCH_FRAME\r\n" );
        pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
        break ;
    case GET_INTERFACE :
        usb_debugstring( "\r\nGET_INTERFACE\r\n" );
        pokec( __USBEP0D_REG, Slave_IfcAlt[dReq.wIndex >> 8] ); // load current alternate setting
        pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_DATA_END | EP0_IN_PKT_RDY ) ; // Endpoint 0 CSR1 register
        break ;
    case SET_INTERFACE :
        usb_debugstring( "\r\nSET_INTERFACE\r\n" );
        Slave_IfcAlt[dReq.wIndex >> 8] = ( BYTE )dReq.wValue;       // selected interface number
        pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
        //pokec(__USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
        pokec( __USBE0C_REG, status | EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
        break ;
    case SET_DESCRIPTOR :
        usb_debugstring( "\r\n Received a SET DESCRIPTOR descriptor" );
        len_req = DEV_LEN ;
        ep0_state = EP0_OUT_DATA_PHASE ;
        break ;
    case SET_FEATURE :
        ep_num = ( dReq.wIndex >> 8 ) & 0x07 ; // EP0 ~ EP5
        switch( dReq.bmRequest & 0x03 )                         // check for recipients
        {
        case RECIPIENT_DEV:
            usb_debugstring( "\r\n Received SET FEATURE (REMOTE_WAKEUP) descriptor" ) ;
            if( ( BYTE )dReq.wValue == 1 )                  // feature selector = 1 (remote wakeup)
            {
                Slave_RemoteWU = 1;
                pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            }
            else
            {
                pokec( __USBIND_REG, EP0 );                    // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SEND_STALL ); // Stall all unsupported requests
                //pokec(__USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            }
            break ;
        case RECIPIENT_ENP:
            usb_debugprintf( "\r\n Received SET FEATURE (ENDPOINT) descriptor for EP %x", ep_num );
            if ( ( BYTE )dReq.wValue == 0 )    // feature selector = 0 (endpoint stall)
            {
                Slave_EPstall |=  ~( 0x01 << ep_num );
                pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
                set_ep_out_stall( ep_num );
            }
            else
            {
                pokec( __USBIND_REG, EP0 );                    // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SEND_STALL ); // Stall all unsupported requests
                //pokec(__USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            }
            break ;
        default :
            usb_debugstring( "\r\n Received SET FEATURE descriptor with invalid Feature" );
            pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
            //pokec(__USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            pokec( __USBE0C_REG, status | EP0_SEND_STALL ); // Stall all unsupported requests
            break;
        }
        break;
    case CLEAR_FEATURE :
        ep_num = ( dReq.wIndex >> 8 ) & 0x07 ; // EP0 ~ EP5
        switch( dReq.bmRequest & 0x03 )                         // check for recipients
        {
        case RECIPIENT_DEV:
            usb_debugstring( "\r\n Received CLEAR FEATURE (REMOTE_WAKEUP) descriptor" ) ;
            if( ( BYTE )dReq.wValue == 1 )                  // feature selector = 1 (remote wakeup)
            {
                Slave_RemoteWU = 0;
                pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            }
            else
            {
                pokec( __USBIND_REG, EP0 );                    // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SEND_STALL ); // Stall all unsupported requests
                //pokec(__USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            }
            break ;
        case RECIPIENT_ENP:
            usb_debugprintf( "\r\n Received CLEAR FEATURE (ENDPOINT) descriptor for EP %x", ep_num );
            if ( ( BYTE )dReq.wValue == 0 )    // feature selector = 0 (endpoint stall)
            {
                Slave_EPstall &=  ~( 0x01 << ep_num );
                pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
                clr_ep_in_stall( ep_num );
                clr_ep_out_stall( ep_num );
            }
            else
            {
                pokec( __USBIND_REG, EP0 );                    // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SEND_STALL ); // Stall all unsupported requests
                //pokec(__USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            }
            break ;
        default :
            usb_debugstring( "\r\n Received CLEAR FEATURE descriptor with invalid Feature" );
            pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
            //pokec(__USBE0C_REG, status | EP0_SERVICED_SETUP_END | EP0_DATA_END ) ; // Endpoint 0 CSR1 register
            pokec( __USBE0C_REG, status | EP0_SEND_STALL ); // Stall all unsupported requests
            break;
        }
        break ;
    case GET_STATUS :
        usb_debugstring( "\r\n Received GET STATUS descriptor" );
        ep_num = ( dReq.wIndex >> 8 ) & 0x07 ; // EP0 ~ EP5
        switch( dReq.bmRequest & 0x03 )                         // check for recipients
        {
        case RECIPIENT_DEV:         // Device status
            pokec( __USBEP0D_REG, ( Slave_RemoteWU << 1 ) | BUS_POWERED );
            pokec( __USBEP0D_REG, 0x00 );
            break ;
        case RECIPIENT_IFC:         // Interface Status
            pokec( __USBEP0D_REG, 0x00 );
            pokec( __USBEP0D_REG, 0x00 );
            break ;
        case RECIPIENT_ENP:         // Endpoint Status
            if ( Slave_EPstall & ( 0x01 << ep_num ) )
                pokec( __USBEP0D_REG, 0x01 ); // ep_num Endpoint Stall
            else
                pokec( __USBEP0D_REG, 0x00 ); // ep_num Endpoint not Stall
            pokec( __USBEP0D_REG, 0x00 );
            break ;
        default :
            pokec( __USBEP0D_REG, 0x00 );
            pokec( __USBEP0D_REG, 0x00 );
            break ;
        }
        pokec( __USBIND_REG, EP0 );                                    // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_DATA_END | EP0_IN_PKT_RDY ) ; // Endpoint 0 CSR register
        break;
    case SET_CONFIG :
        usb_debugstring( "\r\n Received SET CONFIGURATION descriptor" );
        Slave_ConfigVal = ( BYTE )dReq.wValue;
//            usb_debugprintf("\r\n ConfigVal: %x", Slave_ConfigVal);
        pokec( __USBIND_REG, EP0 );                                              // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END ) ; // Endpoint 0 CSR register
        break;
    case GET_CONFIG :
        usb_debugstring( "\r\n Received GET CONFIGURATION descriptor" );
        pokec( __USBEP0D_REG, Slave_ConfigVal );
        pokec( __USBIND_REG, EP0 );                                    // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_DATA_END | EP0_IN_PKT_RDY ) ; // Endpoint 0 CSR register
        break;
    case GET_DESCRIPTOR :
        switch ( dReq.wValue >> 8 )
        {
        case DEVICE:  //01
            usb_debugstring( "\r\n Received GET DEVICE DESCRIPTOR " );
            //len_req = (len_req>=DEV_LEN) ? DEV_LEN:len_req;         // get exact data length
            xmit_len = 0 ;
            if ( len_req > DEV_LEN )
            {
                want_more_data = 1 ;
                len_req = DEV_LEN ;
            }
            else
                want_more_data = 0 ;
            ep0_state = EP0_IN_DATA_PHASE ;
            dec_rom_addr = usb_dev_description;
            WriteEP0();
            break ;
        case CONFIGURATION :
            usb_debugstring( "\r\n Received GET CONFIGURATION DESCRIPTOR " );
            //len_req = (len_req>=CFG_LEN) ? CFG_LEN:len_req;         // get exact data length
            xmit_len = 0 ;
            //BYTE* cfgdescp = usb_get_cfg_description();
            //BYTE* cfgdescp = usb_cfg_description;
            if ( len_req > usb_cfg_description[2] )
            {
                //want_more_data = 1 ;
                //len_req = CFG_LEN;
                len_req = usb_cfg_description[2];
            }
            else
                want_more_data = 0 ;
            ep0_state = EP0_IN_DATA_PHASE ;
            dec_rom_addr = usb_cfg_description;
            WriteEP0();
            break;
        case STRING :
            usb_debugstring( "\r\n Received GET STRING DESCRIPTOR " );
			if(usb_string_description == NULL)
			{
				pokec( __USBIND_REG, EP0 );                                              // Endpoint 0
				pokec( __USBE0C_REG, status | EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END ) ; // Endpoint 0 CSR register
			}
			else
			{
				xmit_len = 0 ;
				if ( len_req > usb_string_description[0] )
				{
					len_req = usb_string_description[0];
				}
				else
					want_more_data = 0 ;
				ep0_state = EP0_IN_DATA_PHASE ;
				dec_rom_addr = usb_string_description;
				WriteEP0();
			}
            break ;
        default :
            usb_debugstring( "Received GET Unknown Descriptor\r\n" );
            pokec( __USBIND_REG, EP0 );                                              // Endpoint 0
            pokec( __USBE0C_REG, status | EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END ) ; // Endpoint 0 CSR register
            break ;
        }
        break;
    case SET_ADDRESS :
        usb_debugstring( "\r\n Received SET ADDRESS descriptor" ) ;
        Slave_USBaddr = ( BYTE )dReq.wValue;
//	        usb_debugprintf("\r\n ADDR = 0x%x",Slave_USBaddr);
        pokec( __USBFA_REG, Slave_USBaddr | ADDR_UPDATA ) ;
        pokec( __USBIND_REG, EP0 );                                              // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END ) ; // Endpoint 0 CSR register
        ep0_state = EP0_IDLE ;
        break;
    default:
        usb_debugprintf( "Error : dReq.bRequest:%#x\r\n", dReq.bRequest );
        while( 1 );
    }
}

void ep0_isr( void )
{
    BYTE byte_rx;
    BYTE status ;
    BYTE req_type ;
    pokec( __USBIND_REG, EP0 );     // Endpoint 0
    status = peekc( __USBE0C_REG ) ; // Endpoint 0 CSR1 register
    //================================================================
    // SENT_STALL
    //================================================================
    if ( status & EP0_SENT_STALL )
    {
        usb_debugstring( "\r\n *** USB CORE SENT STALL HANDSHAKE *** " );
        pokec( __USBIND_REG, EP0 );     // Endpoint 0
        pokec( __USBE0C_REG, status & EP0_CLR_SENT_STALL ) ; // Endpoint 0 CSR1 register
        ep0_state = EP0_IDLE ;
    }
    //================================================================
    // SETUP END
    //================================================================
    if ( status & EP0_SETUP_END )
    {
        usb_debugstring( "\r\n Detected SETUP END" );
        pokec( __USBIND_REG, EP0 );     // Endpoint 0
        pokec( __USBE0C_REG, status | EP0_SERVICED_SETUP_END ) ; // Endpoint 0 CSR1 register
        ep0_state = EP0_IDLE ;
    }
    //================================================================
    //  Processing Control Transfers
    //================================================================
    switch ( ep0_state )
    {
    case EP0_IDLE :
        if ( status & EP0_OUT_PKT_RDY )
        {
            usb_debugstring( "\r\n ENDPT0 RECEIVED OUT PKT RDY " );
            pokec( __USBIND_REG, EP0 );         // Endpoint 0
            byte_rx = peekc( __USBLBOWC_REG );  // RX Bytes
            BufRead( __USBEP0D_REG, ( BYTE* )&dReq, byte_rx );
            len_req = dReq.wLength ; // len_req = actual requested length
            //usb_debugprintf(" wLength %x", len_req) ; // len_req = actual requested length
            if ( len_req > 0 )
            {
                pokec( __USBIND_REG, EP0 );                      // Endpoint 0
                pokec( __USBE0C_REG, status | EP0_SERVICED_OUT_PKT_RDY ) ; // Endpoint 0 CSR register
            }
            // [6:5] Type
            // 0 = Standard
            // 1 = Class
            // 2 = Vendor
            // 3 = Reserved
            req_type = ( dReq.bmRequest & 0x60 ) >> 5;
            usb_debugprintf( "Request type : 0x%x\r\n", req_type );
            switch( req_type )
            {
                //---------------------------------------------------------------------
                // Standard USB Requests
                //---------------------------------------------------------------------
            case STD_REQUEST:    // 00
                ProcessStdDevReq( status );
                break ;
                //---------------------------------------------------------------------
                // Specific Class Requests (HID)
                //---------------------------------------------------------------------
            case CLASS_REQUEST:
                usb_debugstring( "\r\n CLASS REQUEST\r\n" );
                switch ( dReq.bRequest )                            // Parse bRequest
                {
                case BULKONLY_GET_MAX_LUN:
                    usb_debugstring( "\r\n BULKONLY_GET_MAX_LUN\r\n" );
                    pokec( __USBEP0D_REG, LOGIC_UNIT_NUM );
                    pokec( __USBIND_REG, EP0 );                                            // Endpoint 0
                    pokec( __USBE0C_REG, status | EP0_DATA_END | EP0_IN_PKT_RDY ) ; // Endpoint 0 CSR1 register
                    break ;
                case BULKONLY_RESET:
                    usb_debugstring( "\r\n BULKONLY_RESET \r\n" );
                    //usb_reset();
                    //ep0_init();
                    //ep1_init();
                    //ep2_init();
                    //EP1_init();
                    pokec( __USBIND_REG, EP0 );                                              // Endpoint 0
                    pokec( __USBE0C_REG, status | EP0_SERVICED_OUT_PKT_RDY | EP0_DATA_END ) ; // Endpoint 0 CSR register
                    ep0_state = EP0_IDLE ;
                    break;
                default:
                    usb_debugstring( "\r\nCLASS REQUEST : DEFAULT\r\n" );
                }
                break;
                //---------------------------------------------------------------------
                // Specific Vendor Requests
                //---------------------------------------------------------------------
            case VENDOR_REQUEST:
                //ProcessVendorReq();
                break;
                //---------------------------------------------------------------------
                // Unsupported Requests
                //---------------------------------------------------------------------
            case GET_MS_DESCRIPTOR:
            default:
                usb_debugstring( "\r\n Default : Unsupported Requests" );
                pokec( __USBE0C_REG, status | EP0_SEND_STALL ); // Stall all unsupported requests
                break;
            }
        }  // End of IF
        break;
    case EP0_IN_DATA_PHASE :
        WriteEP0();
        break;
    case EP0_OUT_DATA_PHASE :
        usb_debugstring( "\r\n Truly Unload the packet" );
//            ReadEP0();
        break;
    case EP0_END_XFER :
        break ;
    }
}


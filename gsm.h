#ifndef gsm_h
#define gsm_h

#include "Arduino.h"
#include <string.h>
#include <Vendsd.h>
#include <HardwareSerial.h>
#include <FreeRTOS_ARM.h>
#include <error.h>

//////////////defines
#ifndef DEBUG_G
#define DEBUG_G 1
#endif

#define ser Serial1
#define gsm_write(x)   Serial1.print(F(x))
#if DEBUG_G
#define DEBUG_PRINT_F(x)    Serial.print(F(x))
#define DEBUG_PRINT(x)	  Serial.print(x)
#define DEBUGLN_F(x)    Serial.println(F(x))
#define DEBUGLN(x)    Serial.println(x)

#else
#define DEBUG_PRINT_F(x)    
#define DEBUG_PRINT(x)	  
#define DEBUGLN_F(x)    
#define DEBUGLN(x)    
#endif 

#define APN "\"airtelgprs.com\""
#define DETACHED 3
#define TIME_OUT 10000
////////////////////////////////////////////http return codes

#define GET "0"
#define POST "1"


#define IP_INITIAL 4
/////////////////
////////enum
	
	enum command{AT,AT_CPIN,AT_CSQ,AT_COPS,AT_CGSN,AT_CMEE,AT_W,AT_CGATT_q,AT_CGATT,AT_CIPSTATUS,AT_SHUT,AT_CIPMUX,AT_CSTT,AT_CIICR,AT_CIFSR,AT_CIPSRIP,AT_CIPSTART,AT_CIPSEND,
		AT_CIPRECV};
	enum command_http{ AT_SAPBR_ = 30,AT_SAPBR, AT_SAPBR_II, AT_HTTPINIT, AT_HTTPPARA, AT_HTTPACTION, AT_HTTPDATA, AT_HTTPDATA_C, AT_HTTPREAD, AT_HTTPTERM };

////////

class gsm{
	public:
	gsm();
	
	uint8_t at_set();                                                                      //general
	uint8_t sim_status();                                                                  //general
	uint8_t signal_quality();                                                              //general
	uint8_t card_pr();                                                                     //general
	uint8_t imei_no();                                                                     //general
	uint8_t extended_error_code();                                                         //general
	uint8_t http_gsm_init();  
	uint8_t at_http_init();                                                             //htpp conn
	uint8_t http_gsm_term();                                                               //http conn
	uint8_t gsm_init();	                                                                   //tcp conn
	uint8_t connect_tcp(uint8_t mode,const String &addr, const String &port);              //tcp conn
	uint8_t send_data(String &send_str);                                                   //tcp conn
	uint8_t http_set_para(String &url);                                                    //http conn
	uint8_t http_send_data(String &url,String &http_type);                                 //http conn
	uint8_t http_send_data(String &url, String &http_type,String *post_data);              //http conn
	uint8_t http_send_transact(String &url,String &http_type,sd_card *sd_read);            //http conn
	uint8_t send_post_data(uint16_t *data_length, uint16_t *time_out, String *post_data);  //http conn
	uint8_t http_read();                 //http conn
	uint8_t recv_data(String &recv_str); //tcp conn
	uint8_t close_tcp();                 // tcp conn
	uint8_t detach_gprs();               //tcp or http
	uint8_t send_err();                  //error report inheritance derived
	
		
	private:
	uint8_t gen_init();
	uint8_t http_init();
	uint8_t gprs_init();
	uint8_t tcp_init();
	uint8_t check_status(uint8_t command);
	uint8_t check_http_status(uint8_t command_http);
	uint8_t action_parse(String &http_type_got,String &status_code,String &length);
	uint8_t str_parse(String &expected,String &received,String &response);
	uint8_t error_status(uint8_t command,String &response,String &received);

	String recv_priv_data;
	String signal_q;
	String provider;
	String imei;
	};
	
	#endif

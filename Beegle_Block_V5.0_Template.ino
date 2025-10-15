#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include "Nextion.h"
#include <EEPROM.h>
#include <WiFiClientSecure.h>

#define BEEGLE_BOX_VERSION              "Beegle Box V5.0"

#define NONE_AUTOCONNECT                0
#define AUTOCONNECT                     1
#define AUTO_WIFI_WAITING_TIME          500000
#define BLOCK_HIEGHT_ROUND              30
#define BTC_PRICE_ROUND                 30

#define STATE_1_BTC_PRICE               0
#define STATE_2_BLOCK_HIEGHT            1
#define STATE_3_FOREX_RATE              2
#define STATE_4_DATE_TIME               3

#define HUNDRED_MILLION_USD             100000000
#define ZERO_USD                        0
#define THOUSAND_MILLION_THB            1000000000
#define ZERO_THB                        0
#define INIT_USD_RATE                   35.11225
#define MIN_USD_RATE                    10
#define MAX_USD_RATE                    70
#define TEN_MILLION_BLOCK               10000000
#define ZERO_BLOCK                      0

#define NO_DATA_IN_BUFFER               0
#define SMALL_BUFFER_SIZE               512
#define DEFAULT_BUFFER_SIZE             1024
#define BIG_BUFFER_SIZE                 3072
//#define SERIAL_BAUD_RATE                115200
#define SERIAL_BAUD_RATE                9600
#define DECIMAL                         10
#define LED_ON                          1
#define LED_OFF                         0
#define BUFFER_SIZE                     30
#define BASE_TIME_ZONE_ASIA             7
#define STRING_1_DIGIT_DATE             24
#define STRING_2_DIGIT_DATE             25
#define STRING_TIME_DIGIT               5
#define START_TIME_POSITION_1_DIGIT     12
#define START_TIME_POSITION_2_DIGIT     13
#define START_DATE_POSITION             4
#define DIGIT_PER_COMMA                 3
#define POSITION_OF_COMMA               4
#define MAX_SET_OF_COMMA                10
#define FLOAT_CONSTANT                  100
#define PAGE0                           0
#define PAGE1                           1
#define PAGE2                           2
#define PAGE3                           3
#define PAGE4                           4
#define PAGE5                           5
#define PAGE6                           6
#define PAGE7                           7
#define PAGE8                           8
#define PAGE9                           9
#define SSID_SIZE                       50
#define PASSWORD_SIZE                   50
#define EEPROM_SIZE                     200
#define SSID_EEPROM_ADDR                0
#define PASSWORD_EEPROM_ADDR            60
#define AUTO_CONNECT_EEPROM_ADDR        125 //This add for v4.2
#define READ_EEPROM_MAX_SIZE            50
#define INPUT_FOR_SSID                  1
#define INPUT_FOR_PASSWORD              2
#define BACK_SLASH                      92
#define DOUBLE_QUOTE                    34
#define SINGLE_QUOTE                    39
#define SSID_MAX_SIZE                   50
#define PASSWORD_MAX_SIZE               50
#define RESTART_THRESHOLD               5
#define WIFI_WAIT_THRESHOLD             30
#define MID_NIGHT_TIME_BASE             24
#define MIDNIGHT                        0
#define NINE_AM                         9

#define DELAY_PAGE_CHANGE               200
#define DELAY_FOR_LOOP                  10000
#define DELAY_FOR_INIT                  1000
#define ONE_SECOND                      1000
#define THREE_SECOND                    3000
#define DELAY_EEPROM                    1000

// Create a secure client
WiFiClientSecure SwifiClient;
WiFiClient wifiClient;

bool Just_Come_To_Page_9 = true;
uint32_t Auto_Wifi_Waiting_Time = 0;
uint8_t EEPROM_Auto_Connect = 0;
uint32_t Last_Height_Temp = 0;
uint8_t Current_Page = PAGE1;
char Charactor_Array[SSID_MAX_SIZE]; //This array use for arrange alphabet to Wifi SSID
uint8_t Charactor_Index = 0;

uint8_t Input_State;
const size_t bufferSize1 = SMALL_BUFFER_SIZE;
const size_t bufferSize2 = BIG_BUFFER_SIZE;
uint32_t BTC_USD_Rate;
uint8_t State = STATE_1_BTC_PRICE;
uint8_t LED_State = LED_OFF;
float Old_Value_THB = INIT_USD_RATE; //Initial USD exchange rate
float THB_Rate = INIT_USD_RATE;
uint64_t BTC_THB_Price; //This variable carry a big number when calculate the THB price
uint8_t Current_Date = 0; //This variable use for save date in state 1 and use at state 3
uint8_t Old_Date = 0; //This variable use for save date in state 1 and use at state 3
uint8_t Restart_Counter = 0; //This variable us for count when cannot read exchange rate
uint8_t Block_Hieght_Round_Count = BLOCK_HIEGHT_ROUND; //This variable use for prevent too much request from API
uint8_t BTC_Price_Round_Count = BTC_PRICE_ROUND; //This variable use for prevent too much request from API

char ssid[SSID_MAX_SIZE] = {0};
char password[PASSWORD_MAX_SIZE] = {0};

// CoinMarketCap API key and API host and endpoint
const char* host = "pro-api.coinmarketcap.com";
const char* endpoint = "/v1/cryptocurrency/quotes/latest?symbol=BTC&convert=USD";
const char* apiKey = "xxxx";

// BlockCypher API settings
const char* apiHost = "api.blockcypher.com";
const char* apiPath = "/v1/btc/main";
const char* apiToken = "xxxx"; // Your API token

// TimeZoneDB API settings
// Get your API key from https://timezonedb.com/
const char* tzdbApiKey = "xxxx";
const char* tzdbHost   = "api.timezonedb.com";
// Endpoint: get time by zone; for Bangkok, use zone "Asia/Bangkok"
// The URL (using HTTPS) becomes:
// https://api.timezonedb.com/v2.1/get-time-zone?key=YOUR_TIMEZONEDB_API_KEY&format=json&by=zone&zone=Asia/Bangkok
String tzdbEndpoint = "/v2.1/get-time-zone?key=" + String(tzdbApiKey) + "&format=json&by=zone&zone=Asia/Bangkok";

//-------------------------------------------- Function Prototype ----------------------------------------------------//
void Wifi_Set_up_Callback(void *ptr);
void Connect_To_Callback(void *ptr);
void Setup_Wifi_User_Callback(void *ptr); 
void Setup_Wifi_Pass_Callback(void *ptr); 
void Page2_Connect_Callback(void *ptr); 
void Page2_Back_Callback(void *ptr); 
void Q_Button_Callback(void *ptr); 
void W_Button_Callback(void *ptr); 
void E_Button_Callback(void *ptr); 
void R_Button_Callback(void *ptr);
void Add_Comma(char * buff_in, char * buff_out);
void Normal_Operation(void);
void LED_Operation(void);
String EEPROM_read(int index, int length);
uint8_t EEPROM_write(int index, String text);
void IP_Address_Show(void);
void Read_Wifi_Configure_EEPROM(void);
void T_Button_Callback(void *ptr);
void Y_Button_Callback(void *ptr);
void U_Button_Callback(void *ptr);
void I_Button_Callback(void *ptr); 
void O_Button_Callback(void *ptr);
void P_Button_Callback(void *ptr);
void A_Button_Callback(void *ptr);
void S_Button_Callback(void *ptr);
void D_Button_Callback(void *ptr);
void F_Button_Callback(void *ptr);
void G_Button_Callback(void *ptr);
void H_Button_Callback(void *ptr);
void J_Button_Callback(void *ptr);
void K_Button_Callback(void *ptr);
void L_Button_Callback(void *ptr); 
void Shift1_Button_Callback(void *ptr);
void Z_Button_Callback(void *ptr);
void X_Button_Callback(void *ptr);
void C_Button_Callback(void *ptr);
void V_Button_Callback(void *ptr);
void B_Button_Callback(void *ptr);
void N_Button_Callback(void *ptr);
void M_Button_Callback(void *ptr); 
void Delete1_Button_Callback(void *ptr); 
void Back1_Button_Callback(void *ptr);
void Num1_Button_Callback(void *ptr);
void Space1_Button_Callback(void *ptr);
void Next1_Button_Callback(void *ptr); 
void q_Button_Callback(void *ptr); 
void w_Button_Callback(void *ptr);
void e_Button_Callback(void *ptr);
void r_Button_Callback(void *ptr);
void t_Button_Callback(void *ptr); 
void y_Button_Callback(void *ptr); 
void u_Button_Callback(void *ptr);
void i_Button_Callback(void *ptr);
void o_Button_Callback(void *ptr); 
void p_Button_Callback(void *ptr);
void a_Button_Callback(void *ptr);
void s_Button_Callback(void *ptr); 
void d_Button_Callback(void *ptr);
void f_Button_Callback(void *ptr);
void g_Button_Callback(void *ptr);
void h_Button_Callback(void *ptr);
void j_Button_Callback(void *ptr);
void k_Button_Callback(void *ptr);
void l_Button_Callback(void *ptr);
void Shift2_Button_Callback(void *ptr);
void z_Button_Callback(void *ptr);
void x_Button_Callback(void *ptr);
void c_Button_Callback(void *ptr);
void v_Button_Callback(void *ptr);
void b_Button_Callback(void *ptr);
void n_Button_Callback(void *ptr);
void m_Button_Callback(void *ptr);
void Delete2_Button_Callback(void *ptr);
void Back2_Button_Callback(void *ptr);
void Num2_Button_Callback(void *ptr);
void Space2_Button_Callback(void *ptr);
void Next2_Button_Callback(void *ptr); 
void one3_Button_Callback(void *ptr);
void two3_Button_Callback(void *ptr);
void three3_Button_Callback(void *ptr);
void four3_Button_Callback(void *ptr);
void five3_Button_Callback(void *ptr);
void six3_Button_Callback(void *ptr); 
void seven3_Button_Callback(void *ptr);
void eight3_Button_Callback(void *ptr);
void nine3_Button_Callback(void *ptr);
void zero3_Button_Callback(void *ptr); 
void hash_Button_Callback(void *ptr);
void ampersand_Button_Callback(void *ptr); 
void backslash_Button_Callback(void *ptr); 
void slash3_Button_Callback(void *ptr);
void underscore3_Button_Callback(void *ptr); 
void minus_Button_Callback(void *ptr); 
void plus3_Button_Callback(void *ptr);
void left_bracket_Button_Callback(void *ptr);
void right_bracket_Button_Callback(void *ptr);
void Sign3_Button_Callback(void *ptr);
void star_Button_Callback(void *ptr); 
void double_quote_Button_Callback(void *ptr); 
void single_quote_Button_Callback(void *ptr);
void dot_Button_Callback(void *ptr);
void comma_Button_Callback(void *ptr);
void exclamation3_Button_Callback(void *ptr);
void question_Button_Callback(void *ptr);
void Delete3_Button_Callback(void *ptr);
void Back3_Button_Callback(void *ptr);
void Alphabet3_Button_Callback(void *ptr); 
void Space3_Button_Callback(void *ptr);
void Next3_Button_Callback(void *ptr);
void one4_Button_Callback(void *ptr);
void two4_Button_Callback(void *ptr);
void three4_Button_Callback(void *ptr);
void four4_Button_Callback(void *ptr);
void five4_Button_Callback(void *ptr); 
void six4_Button_Callback(void *ptr); 
void seven4_Button_Callback(void *ptr);
void eight4_Button_Callback(void *ptr);
void nine4_Button_Callback(void *ptr);
void zero4_Button_Callback(void *ptr);
void plus4_Button_Callback(void *ptr);
void multiply_Button_Callback(void *ptr);
void divide_Button_Callback(void *ptr); 
void equal_Button_Callback(void *ptr);
void triangle_right_bracket_Button_Callback(void *ptr);
void soft_left_bracket_Button_Callback(void *ptr);
void soft_right_bracket_Button_Callback(void *ptr);
void strong_left_bracket_Button_Callback(void *ptr);
void strong_right_bracket_Button_Callback(void *ptr); 
void Sign4_Button_Callback(void *ptr);
void colon_Button_Callback(void *ptr);
void assign_Button_Callback(void *ptr);
void triangle_left_bracket_Button_Callback(void *ptr);
void dollar_Button_Callback(void *ptr);
void percent_Button_Callback(void *ptr);
void power_Button_Callback(void *ptr);
void semi_colon_Button_Callback(void *ptr); 
void Delete4_Button_Callback(void *ptr);
void Back4_Button_Callback(void *ptr);
void Alphabet4_Button_Callback(void *ptr); 
void Space4_Button_Callback(void *ptr); 
void Next4_Button_Callback(void *ptr);
void AutoReconnect_CheckBox_Callback(void *ptr); //This add for verion 4.2
//-------------------------------------------- Function Prototype ----------------------------------------------------//  

//Declare your Nextion objects - Example (page id = 0, component id = 1, component name = "b0") 
NexText Test_Box = NexText(PAGE0, 2, "t0"); //This line was test
//-------------------------------------------- Wifi Select Page ----------------------------------------------------//
NexButton Wifi_Set_up = NexButton(PAGE1, 2, "b0");
NexButton Connect_To = NexButton(PAGE1, 3, "b1");
NexText Old_Wifi_SSID = NexText(PAGE1, 5, "t1");

//Register a button object to the touch event list.  
NexTouch *nex_listen_list_page1[] = {
  &Wifi_Set_up,
  &Connect_To,
  NULL
};
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Wifi Setup Page -----------------------------------------------------//
NexText Wifi_User = NexText(PAGE2, 8, "t1");
NexText Wifi_Password = NexText(PAGE2, 9, "t3");
NexButton Setup_Wifi_User = NexButton(PAGE2, 2, "b0");
NexButton Setup_Wifi_Pass = NexButton(PAGE2, 5, "b1");
NexButton Page2_Connect = NexButton(PAGE2, 6, "b2");
NexButton Page2_Back = NexButton(PAGE2, 7, "b3");

//Register a button object to the touch event list.  
NexTouch *nex_listen_list_page2[] = {
  &Setup_Wifi_User,
  &Setup_Wifi_Pass,
  &Page2_Connect,
  &Page2_Back,
  NULL
};
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Wifi Connect Page ---------------------------------------------------//
NexText Wifi_Status = NexText(PAGE3, 2, "t0");
NexText Loading = NexText(PAGE3, 3, "t2");
NexText IP_Text = NexText(PAGE3, 4, "t1");
NexText IP_Address_Box = NexText(PAGE3, 5, "t3");
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Input1 Page ---------------------------------------------------------//
NexText Input_Text1 = NexText(PAGE4, 34, "t0");
NexButton Q_Button = NexButton(PAGE4, 1, "b0");
NexButton W_Button = NexButton(PAGE4, 2, "b1");
NexButton E_Button = NexButton(PAGE4, 3, "b2");
NexButton R_Button = NexButton(PAGE4, 4, "b3");
NexButton T_Button = NexButton(PAGE4, 5, "b4");
NexButton Y_Button = NexButton(PAGE4, 6, "b5");
NexButton U_Button = NexButton(PAGE4, 7, "b6");
NexButton I_Button = NexButton(PAGE4, 8, "b7");
NexButton O_Button = NexButton(PAGE4, 9, "b8");
NexButton P_Button = NexButton(PAGE4, 10, "b9");
NexButton A_Button = NexButton(PAGE4, 11, "b10");
NexButton S_Button = NexButton(PAGE4, 12, "b11");
NexButton D_Button = NexButton(PAGE4, 13, "b12");
NexButton F_Button = NexButton(PAGE4, 14, "b13");
NexButton G_Button = NexButton(PAGE4, 15, "b14");
NexButton H_Button = NexButton(PAGE4, 16, "b15");
NexButton J_Button = NexButton(PAGE4, 17, "b16");
NexButton K_Button = NexButton(PAGE4, 18, "b17");
NexButton L_Button = NexButton(PAGE4, 19, "b18");
NexButton Shift1_Button = NexButton(PAGE4, 20, "b19");
NexButton Z_Button = NexButton(PAGE4, 21, "b20");
NexButton X_Button = NexButton(PAGE4, 22, "b21");
NexButton C_Button = NexButton(PAGE4, 23, "b22");
NexButton V_Button = NexButton(PAGE4, 24, "b23");
NexButton B_Button = NexButton(PAGE4, 25, "b24");
NexButton N_Button = NexButton(PAGE4, 26, "b25");
NexButton M_Button = NexButton(PAGE4, 27, "b26");
NexButton Delete1_Button = NexButton(PAGE4, 28, "b27");
NexButton Back1_Button = NexButton(PAGE4, 29, "b28");
NexButton Num1_Button = NexButton(PAGE4, 33, "b29");
NexButton Space1_Button = NexButton(PAGE4, 30, "b31");
NexButton Next1_Button = NexButton(PAGE4, 31, "b34");

//Register a button object to the touch event list.  
NexTouch *nex_listen_list_page4[] = {
  &Q_Button,
  &W_Button,
  &E_Button,
  &R_Button,
  &T_Button,
  &Y_Button,
  &U_Button,
  &I_Button,
  &O_Button,
  &P_Button,
  &A_Button,
  &S_Button,
  &D_Button,
  &F_Button,
  &G_Button,
  &H_Button,
  &J_Button,
  &K_Button,
  &L_Button,
  &Shift1_Button,
  &Z_Button,
  &X_Button,
  &C_Button,
  &V_Button,
  &B_Button,
  &N_Button,
  &M_Button,
  &Delete1_Button,
  &Back1_Button,
  &Num1_Button,
  &Space1_Button,
  &Next1_Button,
  NULL
};
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Input2 Page ---------------------------------------------------------//
NexText Input_Text2 = NexText(PAGE5, 34, "t0");
NexButton q_Button = NexButton(PAGE5, 1, "b0");
NexButton w_Button = NexButton(PAGE5, 2, "b1");
NexButton e_Button = NexButton(PAGE5, 3, "b2");
NexButton r_Button = NexButton(PAGE5, 4, "b3");
NexButton t_Button = NexButton(PAGE5, 5, "b4");
NexButton y_Button = NexButton(PAGE5, 6, "b5");
NexButton u_Button = NexButton(PAGE5, 7, "b6");
NexButton i_Button = NexButton(PAGE5, 8, "b7");
NexButton o_Button = NexButton(PAGE5, 9, "b8");
NexButton p_Button = NexButton(PAGE5, 10, "b9");
NexButton a_Button = NexButton(PAGE5, 11, "b10");
NexButton s_Button = NexButton(PAGE5, 12, "b11");
NexButton d_Button = NexButton(PAGE5, 13, "b12");
NexButton f_Button = NexButton(PAGE5, 14, "b13");
NexButton g_Button = NexButton(PAGE5, 15, "b14");
NexButton h_Button = NexButton(PAGE5, 16, "b15");
NexButton j_Button = NexButton(PAGE5, 17, "b16");
NexButton k_Button = NexButton(PAGE5, 18, "b17");
NexButton l_Button = NexButton(PAGE5, 19, "b18");
NexButton Shift2_Button = NexButton(PAGE5, 20, "b19");
NexButton z_Button = NexButton(PAGE5, 21, "b20");
NexButton x_Button = NexButton(PAGE5, 22, "b21");
NexButton c_Button = NexButton(PAGE5, 23, "b22");
NexButton v_Button = NexButton(PAGE5, 24, "b23");
NexButton b_Button = NexButton(PAGE5, 25, "b24");
NexButton n_Button = NexButton(PAGE5, 26, "b25");
NexButton m_Button = NexButton(PAGE5, 27, "b26");
NexButton Delete2_Button = NexButton(PAGE5, 28, "b27");
NexButton Back2_Button = NexButton(PAGE5, 30, "b28");
NexButton Num2_Button = NexButton(PAGE5, 33, "b29");
NexButton Space2_Button = NexButton(PAGE5, 31, "b31");
NexButton Next2_Button = NexButton(PAGE5, 32, "b34");

//Register a button object to the touch event list.  
NexTouch *nex_listen_list_page5[] = {
  &q_Button,
  &w_Button,
  &e_Button,
  &r_Button,
  &t_Button,
  &y_Button,
  &u_Button,
  &i_Button,
  &o_Button,
  &p_Button,
  &a_Button,
  &s_Button,
  &d_Button,
  &f_Button,
  &g_Button,
  &h_Button,
  &j_Button,
  &k_Button,
  &l_Button,
  &Shift2_Button,
  &z_Button,
  &x_Button,
  &c_Button,
  &v_Button,
  &b_Button,
  &n_Button,
  &m_Button,
  &Delete2_Button,
  &Back2_Button,
  &Num2_Button,
  &Space2_Button,
  &Next2_Button,
  NULL
};
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Input3 Page ---------------------------------------------------------//
NexText Input_Text3 = NexText(PAGE6, 34, "t0");
NexButton one3_Button = NexButton(PAGE6, 1, "b0");
NexButton two3_Button = NexButton(PAGE6, 2, "b1");
NexButton three3_Button = NexButton(PAGE6, 3, "b2");
NexButton four3_Button = NexButton(PAGE6, 4, "b3");
NexButton five3_Button = NexButton(PAGE6, 5, "b4");
NexButton six3_Button = NexButton(PAGE6, 6, "b5");
NexButton seven3_Button = NexButton(PAGE6, 7, "b6");
NexButton eight3_Button = NexButton(PAGE6, 8, "b7");
NexButton nine3_Button = NexButton(PAGE6, 9, "b8");
NexButton zero3_Button = NexButton(PAGE6, 10, "b9");
NexButton hash_Button = NexButton(PAGE6, 11, "b10");
NexButton ampersand_Button = NexButton(PAGE6, 12, "b11");
NexButton backslash_Button = NexButton(PAGE6, 13, "b12");
NexButton slash3_Button = NexButton(PAGE6, 14, "b13");
NexButton underscore3_Button = NexButton(PAGE6, 15, "b14");
NexButton minus_Button = NexButton(PAGE6, 16, "b15");
NexButton plus3_Button = NexButton(PAGE6, 17, "b16");
NexButton left_bracket_Button = NexButton(PAGE6, 18, "b17");
NexButton right_bracket_Button = NexButton(PAGE6, 19, "b18");
NexButton Sign3_Button = NexButton(PAGE6, 20, "b19");
NexButton star_Button = NexButton(PAGE6, 21, "b20");
NexButton double_quote_Button = NexButton(PAGE6, 22, "b21");
NexButton single_quote_Button = NexButton(PAGE6, 23, "b22");
NexButton dot_Button = NexButton(PAGE6, 24, "b23");
NexButton comma_Button = NexButton(PAGE6, 25, "b24");
NexButton exclamation3_Button = NexButton(PAGE6, 26, "b25");
NexButton question_Button = NexButton(PAGE6, 27, "b26");
NexButton Delete3_Button = NexButton(PAGE6, 28, "b27");
NexButton Back3_Button = NexButton(PAGE6, 30, "b28");
NexButton Alphabet3_Button = NexButton(PAGE6, 33, "b29");
NexButton Space3_Button = NexButton(PAGE6, 31, "b31");
NexButton Next3_Button = NexButton(PAGE6, 32, "b34");

//Register a button object to the touch event list.  
NexTouch *nex_listen_list_page6[] = {
  &one3_Button,
  &two3_Button,
  &three3_Button,
  &four3_Button,
  &five3_Button,
  &six3_Button,
  &seven3_Button,
  &eight3_Button,
  &nine3_Button,
  &zero3_Button,
  &hash_Button,
  &ampersand_Button,
  &backslash_Button,
  &slash3_Button,
  &underscore3_Button,
  &minus_Button,
  &plus3_Button,
  &left_bracket_Button,
  &right_bracket_Button,
  &Sign3_Button,
  &star_Button,
  &double_quote_Button,
  &single_quote_Button,
  &dot_Button,
  &comma_Button,
  &exclamation3_Button,
  &question_Button,
  &Delete3_Button,
  &Back3_Button,
  &Alphabet3_Button,
  &Space3_Button,
  &Next3_Button,
  NULL
};
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Input4 Page ---------------------------------------------------------//
NexText Input_Text4 = NexText(PAGE7, 34, "t0");
NexButton one4_Button = NexButton(PAGE7, 1, "b0");
NexButton two4_Button = NexButton(PAGE7, 2, "b1");
NexButton three4_Button = NexButton(PAGE7, 3, "b2");
NexButton four4_Button = NexButton(PAGE7, 4, "b3");
NexButton five4_Button = NexButton(PAGE7, 5, "b4");
NexButton six4_Button = NexButton(PAGE7, 6, "b5");
NexButton seven4_Button = NexButton(PAGE7, 7, "b6");
NexButton eight4_Button = NexButton(PAGE7, 8, "b7");
NexButton nine4_Button = NexButton(PAGE7, 9, "b8");
NexButton zero4_Button = NexButton(PAGE7, 10, "b9");
NexButton plus4_Button = NexButton(PAGE7, 11, "b10");
NexButton multiply_Button = NexButton(PAGE7, 12, "b11");
NexButton divide_Button = NexButton(PAGE7, 13, "b12");
NexButton equal_Button = NexButton(PAGE7, 14, "b13");
NexButton triangle_right_bracket_Button = NexButton(PAGE7, 15, "b14");
NexButton soft_left_bracket_Button = NexButton(PAGE7, 16, "b15");
NexButton soft_right_bracket_Button = NexButton(PAGE7, 17, "b16");
NexButton strong_left_bracket_Button = NexButton(PAGE7, 18, "b17");
NexButton strong_right_bracket_Button = NexButton(PAGE7, 19, "b18");
NexButton Sign4_Button = NexButton(PAGE7, 20, "b19");
NexButton colon_Button = NexButton(PAGE7, 21, "b20");
NexButton assign_Button = NexButton(PAGE7, 22, "b21");
NexButton triangle_left_bracket_Button = NexButton(PAGE7, 23, "b22");
NexButton dollar_Button = NexButton(PAGE7, 24, "b23");
NexButton percent_Button = NexButton(PAGE7, 25, "b24");
NexButton power_Button = NexButton(PAGE7, 26, "b25");
NexButton semi_colon_Button = NexButton(PAGE7, 27, "b26");
NexButton Delete4_Button = NexButton(PAGE7, 28, "b27");
NexButton Back4_Button = NexButton(PAGE7, 30, "b28");
NexButton Alphabet4_Button = NexButton(PAGE7, 33, "b29");
NexButton Space4_Button = NexButton(PAGE7, 31, "b31");
NexButton Next4_Button = NexButton(PAGE7, 32, "b34");

//Register a button object to the touch event list.  
NexTouch *nex_listen_list_page7[] = {
  &one4_Button,
  &two4_Button,
  &three4_Button,
  &four4_Button,
  &five4_Button,
  &six4_Button,
  &seven4_Button,
  &eight4_Button,
  &nine4_Button,
  &zero4_Button,
  &plus4_Button,
  &multiply_Button,
  &divide_Button,
  &equal_Button,
  &triangle_right_bracket_Button,
  &soft_left_bracket_Button,
  &soft_right_bracket_Button,
  &strong_left_bracket_Button,
  &strong_right_bracket_Button,
  &Sign4_Button,
  &colon_Button,
  &assign_Button,
  &triangle_left_bracket_Button,
  &dollar_Button,
  &percent_Button,
  &power_Button,
  &semi_colon_Button,
  &Delete4_Button,
  &Back4_Button,
  &Alphabet4_Button,
  &Space4_Button,
  &Next4_Button,
  NULL
};
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Wifi Connect With Auto Checkbox Page ---------------------------------------------------// 
  NexText Wifi_Status_9 = NexText(PAGE9, 2, "t0");
  NexText Loading_9 = NexText(PAGE9, 3, "t2");
  NexText IP_Text_9 = NexText(PAGE9, 4, "t1");
  NexText IP_Address_Box_9 = NexText(PAGE9, 5, "t3");
  NexCheckbox AutoReconnect_CheckBox = NexCheckbox(PAGE9, 6, "c0"); //This line add for v4.2

  //Register a button object to the touch event list.  
  NexTouch *nex_listen_list_page9[] = {
    &AutoReconnect_CheckBox,
    NULL
  };
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Main Page -----------------------------------------------------------//   This section need to edit page number
NexText Block_Height = NexText(0,6,"t0");
NexText THB_Price = NexText(0,7,"t1");
NexText USD_Price = NexText(0,8,"t2");
NexText DateBox = NexText(0,9,"t3");
NexText TimeBox = NexText(0,10,"t4");
NexText ForexBox = NexText(0,17,"t10");
//------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Page Declaration ----------------------------------------------------//
NexPage page1 = NexPage(1,0,"Wifi_Select");
NexPage page2 = NexPage(2,0,"Wifi_Setup");
NexPage page3 = NexPage(3,0,"Wifi_Connect");
NexPage page4 = NexPage(4,0,"Input1");
NexPage page5 = NexPage(5,0,"Input2");
NexPage page6 = NexPage(6,0,"Input3");
NexPage page7 = NexPage(7,0,"Input4");
NexPage page8 = NexPage(8,0,"Main_Page");
NexPage page9 = NexPage(9,0,"Auto_Wifi");
//------------------------------------------------------------------------------------------------------------------//

void setup(void) 
{
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on

  //Touch screen vector initilaization
  //-------------------------------------------- Call Back For Page 1 ---------------------------------------------------//
  Wifi_Set_up.attachPop(Wifi_Set_up_Callback, &Wifi_Set_up);
  Connect_To.attachPop(Connect_To_Callback, &Connect_To);

  //-------------------------------------------- Call Back For Page 2 ---------------------------------------------------//
  Setup_Wifi_User.attachPop(Setup_Wifi_User_Callback, &Setup_Wifi_User);
  Setup_Wifi_Pass.attachPop(Setup_Wifi_Pass_Callback, &Setup_Wifi_Pass);
  Page2_Connect.attachPop(Page2_Connect_Callback, &Page2_Connect);
  Page2_Back.attachPop(Page2_Back_Callback, &Page2_Back);

  //-------------------------------------------- Call Back For Page 3 ---------------------------------------------------//
  //No touch screen at page 3

  //-------------------------------------------- Call Back For Page 4 ---------------------------------------------------//
  Q_Button.attachPop(Q_Button_Callback, &Q_Button);
  W_Button.attachPop(W_Button_Callback, &W_Button);
  E_Button.attachPop(E_Button_Callback, &E_Button);
  R_Button.attachPop(R_Button_Callback, &R_Button);
  T_Button.attachPop(T_Button_Callback, &T_Button);
  Y_Button.attachPop(Y_Button_Callback, &Y_Button);
  U_Button.attachPop(U_Button_Callback, &U_Button);
  I_Button.attachPop(I_Button_Callback, &I_Button);
  O_Button.attachPop(O_Button_Callback, &O_Button);
  P_Button.attachPop(P_Button_Callback, &P_Button);
  A_Button.attachPop(A_Button_Callback, &A_Button);
  S_Button.attachPop(S_Button_Callback, &S_Button);
  D_Button.attachPop(D_Button_Callback, &D_Button);
  F_Button.attachPop(F_Button_Callback, &F_Button);
  G_Button.attachPop(G_Button_Callback, &G_Button);
  H_Button.attachPop(H_Button_Callback, &H_Button);
  J_Button.attachPop(J_Button_Callback, &J_Button);
  K_Button.attachPop(K_Button_Callback, &K_Button);
  L_Button.attachPop(L_Button_Callback, &L_Button);
  Shift1_Button.attachPop(Shift1_Button_Callback, &Shift1_Button);
  Z_Button.attachPop(Z_Button_Callback, &Z_Button);
  X_Button.attachPop(X_Button_Callback, &X_Button);
  C_Button.attachPop(C_Button_Callback, &C_Button);
  V_Button.attachPop(V_Button_Callback, &V_Button);
  B_Button.attachPop(B_Button_Callback, &B_Button);
  N_Button.attachPop(N_Button_Callback, &N_Button);
  M_Button.attachPop(M_Button_Callback, &M_Button);
  Delete1_Button.attachPop(Delete1_Button_Callback, &Delete1_Button);
  Back1_Button.attachPop(Back1_Button_Callback, &Back1_Button);
  Num1_Button.attachPop(Num1_Button_Callback, &Num1_Button);
  Space1_Button.attachPop(Space1_Button_Callback, &Space1_Button);
  Next1_Button.attachPop(Next1_Button_Callback, &Next1_Button);

  //-------------------------------------------- Call Back For Page 5 ---------------------------------------------------//
  q_Button.attachPop(q_Button_Callback, &q_Button);
  w_Button.attachPop(w_Button_Callback, &w_Button);
  e_Button.attachPop(e_Button_Callback, &e_Button);
  r_Button.attachPop(r_Button_Callback, &r_Button);
  t_Button.attachPop(t_Button_Callback, &t_Button);
  y_Button.attachPop(y_Button_Callback, &y_Button);
  u_Button.attachPop(u_Button_Callback, &u_Button);
  i_Button.attachPop(i_Button_Callback, &i_Button);
  o_Button.attachPop(o_Button_Callback, &o_Button);
  p_Button.attachPop(p_Button_Callback, &p_Button);
  a_Button.attachPop(a_Button_Callback, &a_Button);
  s_Button.attachPop(s_Button_Callback, &s_Button);
  d_Button.attachPop(d_Button_Callback, &d_Button);
  f_Button.attachPop(f_Button_Callback, &f_Button);
  g_Button.attachPop(g_Button_Callback, &g_Button);
  h_Button.attachPop(h_Button_Callback, &h_Button);
  j_Button.attachPop(j_Button_Callback, &j_Button);
  k_Button.attachPop(k_Button_Callback, &k_Button);
  l_Button.attachPop(l_Button_Callback, &l_Button);
  Shift2_Button.attachPop(Shift2_Button_Callback, &Shift2_Button);
  z_Button.attachPop(z_Button_Callback, &z_Button);
  x_Button.attachPop(x_Button_Callback, &x_Button);
  c_Button.attachPop(c_Button_Callback, &c_Button);
  v_Button.attachPop(v_Button_Callback, &v_Button);
  b_Button.attachPop(b_Button_Callback, &b_Button);
  n_Button.attachPop(n_Button_Callback, &n_Button);
  m_Button.attachPop(m_Button_Callback, &m_Button);
  Delete2_Button.attachPop(Delete2_Button_Callback, &Delete2_Button);
  Back2_Button.attachPop(Back2_Button_Callback, &Back2_Button);
  Num2_Button.attachPop(Num2_Button_Callback, &Num2_Button);
  Space2_Button.attachPop(Space2_Button_Callback, &Space2_Button);
  Next2_Button.attachPop(Next2_Button_Callback, &Next2_Button);

  //-------------------------------------------- Call Back For Page 6 ---------------------------------------------------//
  one3_Button.attachPop(one3_Button_Callback, &one3_Button);
  two3_Button.attachPop(two3_Button_Callback, &two3_Button);
  three3_Button.attachPop(three3_Button_Callback, &three3_Button);
  four3_Button.attachPop(four3_Button_Callback, &four3_Button);
  five3_Button.attachPop(five3_Button_Callback, &five3_Button);
  six3_Button.attachPop(six3_Button_Callback, &six3_Button);
  seven3_Button.attachPop(seven3_Button_Callback, &seven3_Button);
  eight3_Button.attachPop(eight3_Button_Callback, &eight3_Button);
  nine3_Button.attachPop(nine3_Button_Callback, &nine3_Button);
  zero3_Button.attachPop(zero3_Button_Callback, &zero3_Button);
  hash_Button.attachPop(hash_Button_Callback, &hash_Button);
  ampersand_Button.attachPop(ampersand_Button_Callback, &ampersand_Button);
  backslash_Button.attachPop(backslash_Button_Callback, &backslash_Button);
  slash3_Button.attachPop(slash3_Button_Callback, &slash3_Button);
  underscore3_Button.attachPop(underscore3_Button_Callback, &underscore3_Button);
  minus_Button.attachPop(minus_Button_Callback, &minus_Button);
  plus3_Button.attachPop(plus3_Button_Callback, &plus3_Button);
  left_bracket_Button.attachPop(left_bracket_Button_Callback, &left_bracket_Button);
  right_bracket_Button.attachPop(right_bracket_Button_Callback, &right_bracket_Button);
  Sign3_Button.attachPop(Sign3_Button_Callback, &Sign3_Button);
  star_Button.attachPop(star_Button_Callback, &star_Button);
  double_quote_Button.attachPop(double_quote_Button_Callback, &double_quote_Button);
  single_quote_Button.attachPop(single_quote_Button_Callback, &single_quote_Button);
  dot_Button.attachPop(dot_Button_Callback, &dot_Button);
  comma_Button.attachPop(comma_Button_Callback, &comma_Button);
  exclamation3_Button.attachPop(exclamation3_Button_Callback, &exclamation3_Button);
  question_Button.attachPop(question_Button_Callback, &question_Button);
  Delete3_Button.attachPop(Delete3_Button_Callback, &Delete3_Button);
  Back3_Button.attachPop(Back3_Button_Callback, &Back3_Button);
  Alphabet3_Button.attachPop(Alphabet3_Button_Callback, &Alphabet3_Button);
  Space3_Button.attachPop(Space3_Button_Callback, &Space3_Button);
  Next3_Button.attachPop(Next3_Button_Callback, &Next3_Button);

  //-------------------------------------------- Call Back For Page 7 ---------------------------------------------------//
  one4_Button.attachPop(one4_Button_Callback, &one4_Button);
  two4_Button.attachPop(two4_Button_Callback, &two4_Button);
  three4_Button.attachPop(three4_Button_Callback, &three4_Button);
  four4_Button.attachPop(four4_Button_Callback, &four4_Button);
  five4_Button.attachPop(five4_Button_Callback, &five4_Button);
  six4_Button.attachPop(six4_Button_Callback, &six4_Button);
  seven4_Button.attachPop(seven4_Button_Callback, &seven4_Button);
  eight4_Button.attachPop(eight4_Button_Callback, &eight4_Button);
  nine4_Button.attachPop(nine4_Button_Callback, &nine4_Button);
  zero4_Button.attachPop(zero4_Button_Callback, &zero4_Button);
  plus4_Button.attachPop(plus4_Button_Callback, &plus4_Button);
  multiply_Button.attachPop(multiply_Button_Callback, &multiply_Button);
  divide_Button.attachPop(divide_Button_Callback, &divide_Button);
  equal_Button.attachPop(equal_Button_Callback, &equal_Button);
  triangle_right_bracket_Button.attachPop(triangle_right_bracket_Button_Callback, &triangle_right_bracket_Button);
  soft_left_bracket_Button.attachPop(soft_left_bracket_Button_Callback, &soft_left_bracket_Button);
  soft_right_bracket_Button.attachPop(soft_right_bracket_Button_Callback, &soft_right_bracket_Button);
  strong_left_bracket_Button.attachPop(strong_left_bracket_Button_Callback, &strong_left_bracket_Button);
  strong_right_bracket_Button.attachPop(strong_right_bracket_Button_Callback, &strong_right_bracket_Button);
  Sign4_Button.attachPop(Sign4_Button_Callback, &Sign4_Button);
  colon_Button.attachPop(colon_Button_Callback, &colon_Button);
  assign_Button.attachPop(assign_Button_Callback, &assign_Button);
  triangle_left_bracket_Button.attachPop(triangle_left_bracket_Button_Callback, &triangle_left_bracket_Button);
  dollar_Button.attachPop(dollar_Button_Callback, &dollar_Button);
  percent_Button.attachPop(percent_Button_Callback, &percent_Button);
  power_Button.attachPop(power_Button_Callback, &power_Button);
  semi_colon_Button.attachPop(semi_colon_Button_Callback, &semi_colon_Button);
  Delete4_Button.attachPop(Delete4_Button_Callback, &Delete4_Button);
  Back4_Button.attachPop(Back4_Button_Callback, &Back4_Button);
  Alphabet4_Button.attachPop(Alphabet4_Button_Callback, &Alphabet4_Button);
  Space4_Button.attachPop(Space4_Button_Callback, &Space4_Button);
  Next4_Button.attachPop(Next4_Button_Callback, &Next4_Button);

  //-------------------------------------------- Call Back For Page 8 ---------------------------------------------------//
  //No touch screen at page 8

  //-------------------------------------------- Call Back For Page 9 ---------------------------------------------------//
  AutoReconnect_CheckBox.attachPop(AutoReconnect_CheckBox_Callback, &AutoReconnect_CheckBox);

  //Init EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  //Waiting for LCD ready
  delay(ONE_SECOND);

  //Nextion LCD initialization
  Serial.begin(SERIAL_BAUD_RATE);
  nexInit(); 

  Test_Box.setText(BEEGLE_BOX_VERSION);

  delay(THREE_SECOND);
  page1.show();
  delay(DELAY_PAGE_CHANGE);
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Old_Wifi_SSID.setText(&charBuf[0]);

  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off

} //End setup

void loop(void) 
{
  switch(Current_Page)
  {
    case PAGE1:
    {
      //EEPROM.write(AUTO_CONNECT_EEPROM_ADDR, NONE_AUTOCONNECT); //This line is for test EEPROM auto connect
      //EEPROM.commit();
      //delay(DELAY_EEPROM);
      
      EEPROM_Auto_Connect = EEPROM.read(AUTO_CONNECT_EEPROM_ADDR);
      if(EEPROM_Auto_Connect == AUTOCONNECT)
      {
        Read_Wifi_Configure_EEPROM();
        page9.show();
        delay(DELAY_PAGE_CHANGE);
        nexLoop(nex_listen_list_page9);
        Current_Page = PAGE9;
      }
      else
      {
        //Repeat calling vector
        nexLoop(nex_listen_list_page1);
      }
      break;
    }

    case PAGE2:
    {
      //Repeat calling vector
      nexLoop(nex_listen_list_page2);
      break;
    }

    case PAGE3:
    {    
      IP_Address_Show();
      page8.show();
      Current_Page = PAGE8;
      break;
    }

    case PAGE4:
    {
      //Repeat calling vector
      nexLoop(nex_listen_list_page4); //Input 1 page
      break;
    }

    case PAGE5:
    {
      //Repeat calling vector
      nexLoop(nex_listen_list_page5); //Input 2 page
      break;
    }

    case PAGE6:
    {
      //Repeat calling vector
      nexLoop(nex_listen_list_page6); //Input 3 page
      break;
    }

    case PAGE7:
    {
      //Repeat calling vector
      nexLoop(nex_listen_list_page7); //Input 4 page
      break;
    }

    case PAGE8:
    {
      Normal_Operation();
      break;
    }

    case PAGE9:
    {
      if(Just_Come_To_Page_9 == true)
      {
        //Get into here just once
        uint8_t CheckBoxValue = EEPROM.read(AUTO_CONNECT_EEPROM_ADDR);
        if(CheckBoxValue == NONE_AUTOCONNECT)
        {
          AutoReconnect_CheckBox.setValue(NONE_AUTOCONNECT);
        }
        else if(CheckBoxValue == AUTOCONNECT)
        {
          AutoReconnect_CheckBox.setValue(AUTOCONNECT);
        }
        Just_Come_To_Page_9 = false;
      } //End if
      
      nexLoop(nex_listen_list_page9);
      
      if(Auto_Wifi_Waiting_Time >= AUTO_WIFI_WAITING_TIME)
      {
        Read_Wifi_Configure_EEPROM();
        page3.show();
        delay(DELAY_PAGE_CHANGE);
        Current_Page = PAGE3;
      }
      Auto_Wifi_Waiting_Time++;
      break;
    }

    default:
    {
      //Null
    }
  } //End switch
  
} //End function

void Add_Comma(char * buff_in, char * buff_out)
{
  int8_t i;
  uint8_t k = 0, j = 0;
  uint8_t Comma_Count = 1; //This have to start from 1 only
  uint8_t Len_Of_USD_rate = strlen(buff_in);
  uint8_t Amount_OF_Comma = (Len_Of_USD_rate - 1) / DIGIT_PER_COMMA;
  char TempBuff_Matrix[MAX_SET_OF_COMMA][DIGIT_PER_COMMA] = {0};
  for(i = 0; i < (Amount_OF_Comma + 1); i++) //Collect rear number after comma
  {
    for(j = 0; j < DIGIT_PER_COMMA; j++)
    {
      TempBuff_Matrix[i][2 - j] = buff_in[(Len_Of_USD_rate - (DIGIT_PER_COMMA * (1 + i))) + j];
    }
  }

  for(i = 0; i < Len_Of_USD_rate / POSITION_OF_COMMA; i++) //Collect front number before comma
  {
    TempBuff_Matrix[Amount_OF_Comma + 1][i] = buff_in[i];
  }
  
//  char TempBuff_Arrange[30] = {0};
  k = 0; 
  j = 0;
  Comma_Count = 1;
  
  for(i = (Len_Of_USD_rate + Amount_OF_Comma) - 1; i >= 0; i--)
  {
    if(Comma_Count % POSITION_OF_COMMA == 0)
    {
      //Serial.println("If");
      j = 0;
      buff_out[i] = ',';
      k++;
    }
    else
    {
      //Serial.println("Else");
      buff_out[i] = TempBuff_Matrix[k][j];
      j++;
    }
    Comma_Count++;
  } //End for
          
} //End function

void Normal_Operation(void)
{
  uint32_t blockHeight = 0;
  char UART_buff[BUFFER_SIZE] = {0}; //This buffer use for nextion
  char UART_buff_comma[BUFFER_SIZE] = {0}; //This buffer use for nextion
  int httpCode;
  HTTPClient http; //Object of class HTTPClient
  uint8_t i;
  
  if (WiFi.status() == WL_CONNECTED) 
  {
    switch(State)
    {
      case STATE_1_BTC_PRICE:
      {
        //This if statement is for prevent too much request from API
        //This is set for 30 mins then get 1 request
        if(BTC_Price_Round_Count >= BTC_PRICE_ROUND)
        {
        
        if (!SwifiClient.connect(host, 443)) {
    //        Serial.println("Connection failed!");
            return;
        }
    
        // Send HTTP GET request
        SwifiClient.print(String("GET ") + endpoint + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "X-CMC_PRO_API_KEY: " + apiKey + "\r\n" +
                     "Accept: application/json\r\n" +
                     "Connection: close\r\n\r\n");
    
        // Wait for response
        while (SwifiClient.available() == 0) {
            delay(100);
        }
    
        // Read HTTP response
        String response;
        while (SwifiClient.available()) {
            response += SwifiClient.readString();
        }
        SwifiClient.stop();
    
        // Print raw response for debugging
    //    Serial.println("Raw API Response:");
    //    Serial.println(response);
    
        // Find the start of the valid JSON by looking for {"status"
        int jsonStart = response.indexOf("{\"status");
        if (jsonStart == -1) {
    //        Serial.println("Error: JSON start not found.");
            return;
        }
    
        // Extract the JSON part
        String jsonResponse = response.substring(jsonStart);
    
        // Remove unwanted characters, e.g., "0\r"
        jsonResponse.replace("0\r", "");
    
        // Print the cleaned JSON response
    //    Serial.println("Cleaned JSON Response:");
    //    Serial.println(jsonResponse);
    
        // Now you can parse the cleaned JSON
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(jsonResponse);
    
        if (root.success()) {
    //        Serial.println("JSON parsed successfully!");
    
            // Extract Bitcoin price
            BTC_USD_Rate = root["data"]["BTC"]["quote"]["USD"]["price"];
              
            //Prevent null value to display
            if(BTC_USD_Rate > ZERO_USD && BTC_USD_Rate < HUNDRED_MILLION_USD)
            {
              sprintf(&UART_buff[0], "%d", BTC_USD_Rate); //Convert uint32_t to string
              Add_Comma(&UART_buff[0] ,&UART_buff_comma[0]); //This section use for add comma separate 3 digit
              USD_Price.setText(UART_buff_comma); //Print USD to text box
            } //End if
            
            } //End if for price calculation

          BTC_Price_Round_Count = 0;
          
        } //End if BTC_PRICE_ROUND_Count

        BTC_Price_Round_Count++;
        State = STATE_2_BLOCK_HIEGHT; // Comment this for not change state
        break;
      } //End case 0

      case STATE_2_BLOCK_HIEGHT:
      {
        //This if statement is for prevent too much request from API
        //This is set for 30 mins then get 1 request
        if(Block_Hieght_Round_Count >= BLOCK_HIEGHT_ROUND)
        {
          if (!SwifiClient.connect(apiHost, 443)) 
          {
            //Serial.println("Connection failed!");
            //return;
          }

          // Build the full API path with the token
          String fullPath = String(apiPath) + "?token=" + apiToken;
      
          // Send HTTP GET request
          SwifiClient.print(String("GET ") + fullPath + " HTTP/1.1\r\n" + "Host: " + apiHost + "\r\n" + "Connection: close\r\n\r\n");
        
          // Wait for response
          while (SwifiClient.connected() && !SwifiClient.available()) 
          {
            delay(100);
          }
        
          // Read response
          String response;
          String height;
          while (SwifiClient.available()) 
          {
            response += SwifiClient.readString();
          }
        
          SwifiClient.stop();

          // Find JSON payload start (skip HTTP headers)
          int jsonStart = response.indexOf("\r\n\r\n");
          String jsonResponse = response.substring(jsonStart + 4);
        
          // Parse JSON using DynamicJsonBuffer
          DynamicJsonBuffer jsonBuffer2(bufferSize1);
          JsonObject& root2 = jsonBuffer2.parseObject(jsonResponse);
        
          if(root2.containsKey("height"))
          {
            String height_temp = root2["height"];
            height = height_temp;
          }

          //Convert block height from string to uint32
          char String_Temp[20] = {0};
          sprintf(&String_Temp[0], "%s", height);
          uint32_t Height_Temp = atoi(String_Temp);

          //Prevent null value to display
          if(Height_Temp > ZERO_BLOCK && Height_Temp < TEN_MILLION_BLOCK)
          {
            sprintf(&UART_buff[0], "%s", height); //Convert string to string
            Add_Comma(&UART_buff[0] ,&UART_buff_comma[0]);
            Block_Height.setText(UART_buff_comma); //Print latest block number to display
          }
          
          http.end(); //Close connection
          Block_Hieght_Round_Count = 0;
          
        } //End if Block_Hieght_Round_Count

        Block_Hieght_Round_Count++;
        State = STATE_3_FOREX_RATE;
        break;
      } //End case 1

      case STATE_3_FOREX_RATE:
      {
        DynamicJsonBuffer jsonBuffer3(DEFAULT_BUFFER_SIZE);
        //Check for reading once a day because API is limit per month
        if(Current_Date != Old_Date)
        {
          http.begin(SwifiClient, "https://v6.exchangerate-api.com/v6/xxxx/latest/USD");
          httpCode = http.GET();
          if (httpCode > NO_DATA_IN_BUFFER) 
          {
            JsonObject& root3 = jsonBuffer3.parseObject(http.getString());         
            JsonObject& conversionRates = root3["conversion_rates"];
            THB_Rate = conversionRates["THB"]; //This variable contain data like "THB":34.90026"
            
          } //End if for EURO rate
          
          http.end(); //Close connection

          Old_Date = Current_Date; //Set current date after read exchange rate, this is for reading once a day
        } //End check date if

        uint32_t THB_Rate_Temp = 0; //This variable for multiply THB rate and make it not floating point

        //Check exchange rate range, because json not read completely sometimes
        if(THB_Rate > MIN_USD_RATE && THB_Rate < MAX_USD_RATE)
        {
          THB_Rate_Temp = THB_Rate * FLOAT_CONSTANT; //Multiply THB rate for make it not float
          BTC_THB_Price = (THB_Rate_Temp * BTC_USD_Rate) / FLOAT_CONSTANT; //Calculate BTC in THB price
          sprintf(&UART_buff[0], "%.2f", THB_Rate); //Convert uint32_t to string
          ForexBox.setText(UART_buff); //Display USD/THB rate
          Old_Value_THB = THB_Rate; //Store exchange rate for prevent the rate not in range
        }
        else
        {
          THB_Rate_Temp = Old_Value_THB * FLOAT_CONSTANT; //Multiply THB rate for make it not float
          BTC_THB_Price = (THB_Rate_Temp * BTC_USD_Rate) / FLOAT_CONSTANT; //Use old value if exchange rate cannot read from server
          sprintf(&UART_buff[0], "%.2f", Old_Value_THB); //Convert uint32_t to string
          ForexBox.setText(UART_buff); //Display USD/THB rate
        }

        //For prevent BTC price is 0 THB
        if(BTC_THB_Price > ZERO_THB && BTC_THB_Price < THOUSAND_MILLION_THB)
        {
          sprintf(&UART_buff[0], "%d", BTC_THB_Price); //Convert uint32_t to string
          Add_Comma(&UART_buff[0] ,&UART_buff_comma[0]);
          THB_Price.setText(UART_buff_comma); //Print USD price to display
        }

        State = STATE_4_DATE_TIME;
        break;
      } //End case 2

      case STATE_4_DATE_TIME:
      {
          // Build the full URL for the API request
          String url = "https://" + String(tzdbHost) + tzdbEndpoint;
        
          // Start the HTTP request
          http.begin(SwifiClient, url);
          int httpCode = http.GET();
        
          if (httpCode == 200) {
            // Get the raw response payload
            String payload = http.getString();
        
          // Parse JSON using ArduinoJson 5.x.x
          DynamicJsonBuffer jsonBuffer(1024);
          JsonObject& root = jsonBuffer.parseObject(payload);
          if (root.success()) {
            // The TimeZoneDB API returns a field "formatted" like "2025-02-10 10:01:00"
            String formattedTime = root["formatted"];
            
            // Extract the year, month, and day from the formatted string
            String year  = formattedTime.substring(0, 4);  // Characters 0-3
            String month = formattedTime.substring(5, 7);   // Characters 5-6
            String day   = formattedTime.substring(8, 10);  // Characters 8-9
            String Time   = formattedTime.substring(11, 16); // Characters 11-15
      
            // Convert month number to a three-letter abbreviation
            const char* monthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
            int monthNum = month.toInt();  // Convert month string to integer (e.g., "02" becomes 2)
            String monthAbbr = monthNames[monthNum - 1];  // Get the corresponding abbreviation
      
            Current_Date = atoi(day.c_str()); //This variable use for save date in state 1 and use at state 3

            sprintf(&UART_buff[0], "%s %s %s", day, monthAbbr, year); //Convert uint32_t to string
            DateBox.setText(UART_buff);
    
            delay(1000);
    
            sprintf(&UART_buff[0], "%s", Time); //Convert uint32_t to string
            TimeBox.setText(UART_buff);

          } 
          else 
          {
//            Serial.println("Failed to parse JSON.");
          }
        } 
        else 
        {
//          Serial.print("HTTP request failed, error code: ");
//          Serial.println(httpCode);
        }
      
        http.end(); // Close the connection
            
        State = STATE_1_BTC_PRICE;
        break;
      } //End case 3

      default:
      {
        //Null
      }
      
    } //End switch   
  } //End if for check wifi connect
  
  LED_Operation();

  delay(DELAY_FOR_LOOP); //Delay for leave some time to read data from server

} //End function

void LED_Operation(void)
{
  if(LED_State == LED_OFF)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on
    LED_State = LED_ON;
  } //End if
  else
  {
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
    LED_State = LED_OFF;
  } //End else
  
} //End function

String EEPROM_read(int index, int length) 
{
  String text = "";
  char ch = 1;
  
  for (uint8_t i = index; (i < (index + length)) && ch; ++i) 
  {
    if (ch = EEPROM.read(i)) 
      {
        text.concat(ch);
      } //End if
  } //End for
  
  return text;
  
} //End function

uint8_t EEPROM_write(int index, String text) 
{
  for (uint8_t i = index; i < text.length() + index; ++i) 
  {
    EEPROM.write(i, text[i - index]);
  } //End for
  
  EEPROM.write(index + text.length(), 0);
  EEPROM.commit();
  
  return text.length() + 1;
  
} //End function

void IP_Address_Show(void)
{
  WiFi.begin(ssid, password);
  //Block_Height.setText("Connecting");

  char TempLoad[30] = {0};
  uint8_t Dot_Count = 0;
  TempLoad[Dot_Count] = '.';
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(DELAY_FOR_INIT);
    Loading.setText(TempLoad);
    Dot_Count++;
    TempLoad[Dot_Count] = '.';

    //This is cannot connect to wifi
    //Lock in this loop
    if(Dot_Count == WIFI_WAIT_THRESHOLD)
    {
      Wifi_Status.setText("Cannot connect WiFi");
      Loading.setText("Check SSID or Password");
      while(1)
      {
        //This code below is use for infinite loop, if we not do this ESP8266 will reset itself
        Loading.setText("Check SSID or Password");
        delay(DELAY_FOR_LOOP);
      } //End while
    } //End if
  } //End outer while

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Tar Test V3.5

  //Set SSL certificate validation (ignore certificates for simplicity)
  SwifiClient.setInsecure();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Tar Test V3.5

  Wifi_Status.setText("WiFi connected");
  IP_Text.setText("IP address is");
  IPAddress IP_Address = WiFi.localIP();
  char Temp_IP_All[40] = {0};
  char Temp_IP1[5] = {0};
  char Temp_IP2[5] = {0};
  char Temp_IP3[5] = {0};
  char Temp_IP4[5] = {0};
  uint8_t IP_Len1 = 0;
  uint8_t IP_Len2 = 0;
  uint8_t IP_Len3 = 0;
  uint8_t IP_Len4 = 0;
  uint8_t i;
  sprintf(&Temp_IP1[0], "%d", IP_Address[0]);
  IP_Len1 = strlen(Temp_IP1);
  for(i = 0; i < IP_Len1; i++)
  {
    Temp_IP_All[i] = Temp_IP1[i];
  }
  Temp_IP_All[IP_Len1] = '.';
  sprintf(&Temp_IP2[0], "%d", IP_Address[1]);
  IP_Len2 = strlen(Temp_IP2);
  for(i = 0; i < IP_Len2; i++)
  {
    Temp_IP_All[IP_Len1 + i + 1] = Temp_IP2[i];
  }
  Temp_IP_All[IP_Len1 + IP_Len2 + 1] = '.';
  sprintf(&Temp_IP3[0], "%d", IP_Address[2]);
  IP_Len3 = strlen(Temp_IP3);
  for(i = 0; i < IP_Len3; i++)
  {
    Temp_IP_All[IP_Len2 + IP_Len1 + i + 2] = Temp_IP3[i];
  }
  Temp_IP_All[IP_Len1 + IP_Len2 + IP_Len3 + 2] = '.';
  sprintf(&Temp_IP4[0], "%d", IP_Address[3]);
  IP_Len4 = strlen(Temp_IP4);
  for(i = 0; i < IP_Len4; i++)
  {
    Temp_IP_All[IP_Len3 + IP_Len2 + IP_Len1 + i + 3] = Temp_IP4[i];
  }
  IP_Address_Box.setText(Temp_IP_All);

  //Wait for show IP address
  delay(THREE_SECOND);
  
} //End function

void Read_Wifi_Configure_EEPROM(void)
{
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for SSID
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  strcpy(ssid, charBuf);

  EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for password
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  strcpy(password, charBuf);

  EEPROM_Temp = EEPROM_read(AUTO_CONNECT_EEPROM_ADDR, 1); //Read bit from auto re-connect just only 1 byte
  EEPROM_Auto_Connect = atoi(EEPROM_Temp.c_str());  // Convert the string to uint8_t
  
} //End function

//-------------------------------------------- Call Back For Page 1 ---------------------------------------------------//

void Wifi_Set_up_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for SSID
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_User.setText(&charBuf[0]);

  EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for password
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_Password.setText(&charBuf[0]);
  
} //End function

void Connect_To_Callback(void *ptr) 
{
  Read_Wifi_Configure_EEPROM();
  page9.show();
  Current_Page = PAGE9;
  delay(DELAY_PAGE_CHANGE);
} //End function

//---------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Call Back For Page 2 ---------------------------------------------------//
void Setup_Wifi_User_Callback(void *ptr) 
{
  page5.show();
  Current_Page = PAGE5;
  Charactor_Index = 0;
  Input_State = INPUT_FOR_SSID;

  //Clear variable
  Charactor_Array[0] = ' ';
  for(uint8_t i = 1; i < SSID_SIZE; i++)
  {
    Charactor_Array[i] = '\0';
  } //End for
} //End function

void Setup_Wifi_Pass_Callback(void *ptr) 
{
  page5.show();
  Current_Page = PAGE5;
  Charactor_Index = 0;
  Input_State = INPUT_FOR_PASSWORD;

  //Clear variable
  Charactor_Array[0] = ' ';
  for(uint8_t i = 1; i < SSID_SIZE; i++)
  {
    Charactor_Array[i] = '\0';
  } //End for
} //End function

void Page2_Connect_Callback(void *ptr) 
{
  Read_Wifi_Configure_EEPROM();
  page9.show();
  Current_Page = PAGE9;
  delay(DELAY_PAGE_CHANGE);
} //End function

void Page2_Back_Callback(void *ptr) 
{
  page1.show();
  Current_Page = PAGE1;
  delay(DELAY_PAGE_CHANGE);
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Old_Wifi_SSID.setText(&charBuf[0]);
} //End function

//-------------------------------------------- Call Back For Page 4 ---------------------------------------------------//
void Q_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'Q';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(&Charactor_Array[0]);
} //End function

void W_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'W';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void E_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'E';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void R_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'R';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void T_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'T';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Y_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'Y';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void U_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'U';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void I_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'I';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void O_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'O';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void P_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'P';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void A_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'A';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void S_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'S';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void D_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'D';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void F_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'F';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void G_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'G';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void H_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'H';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void J_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'J';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void K_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'K';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void L_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'L';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Shift1_Button_Callback(void *ptr) 
{
  page5.show();
  Current_Page = PAGE5;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(&Charactor_Array[0]);
} //End function

void Z_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'Z';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void X_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'X';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void C_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'C';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void V_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'V';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void B_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'B';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void N_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'N';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void M_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'M';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Delete1_Button_Callback(void *ptr) 
{
  //Prevent minus index
  if(Charactor_Index > 0)
  {
    Charactor_Index--; //Delete alphabet before null
  }
  else
  {
    Charactor_Index = 0;
  }
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Back1_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for SSID
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_User.setText(&charBuf[0]);

  EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for password
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_Password.setText(&charBuf[0]);
} //End function

void Num1_Button_Callback(void *ptr) 
{
  page6.show();
  Current_Page = PAGE6;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(&Charactor_Array[0]);
} //End function

void Space1_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ' ';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Next1_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  //Write data into eeprom
  String myString = String(Charactor_Array); //Convert string to char
  if(Input_State == INPUT_FOR_SSID)
  {
    strcpy(ssid, Charactor_Array);
    EEPROM_write(SSID_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End if
  else if(Input_State == INPUT_FOR_PASSWORD)
  {
    strcpy(password, Charactor_Array);
    EEPROM_write(PASSWORD_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End else if

  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_User.setText(&charBuf[0]);

  EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_Password.setText(&charBuf[0]);

} //End function
//---------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Call Back For Page 5 ---------------------------------------------------//
void q_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'q';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void w_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'w';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void e_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'e';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void r_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'r';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void t_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 't';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void y_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'y';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void u_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'u';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void i_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'i';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void o_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'o';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void p_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'p';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void a_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'a';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void s_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 's';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void d_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'd';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void f_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'f';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void g_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'g';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void h_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'h';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void j_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'j';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void k_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'k';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void l_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'l';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Shift2_Button_Callback(void *ptr) 
{
  page4.show();
  Current_Page = PAGE4;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(Charactor_Array);
} //End function

void z_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'z';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void x_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'x';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void c_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'c';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void v_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'v';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void b_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'b';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void n_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'n';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void m_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'm';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Delete2_Button_Callback(void *ptr) 
{
  //Prevent minus index
  if(Charactor_Index > 0)
  {
    Charactor_Index--; //Delete alphabet before null
  }
  else
  {
    Charactor_Index = 0;
  }
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Back2_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for SSID
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_User.setText(&charBuf[0]);

  EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for password
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_Password.setText(&charBuf[0]);
} //End function

void Num2_Button_Callback(void *ptr) 
{
  page6.show();
  Current_Page = PAGE6;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(Charactor_Array);
} //End function

void Space2_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ' ';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Next2_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  //Write data into eeprom
  String myString = String(Charactor_Array); //Convert string to char
  if(Input_State == INPUT_FOR_SSID)
  {
    strcpy(ssid, Charactor_Array);
    EEPROM_write(SSID_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End if
  else if(Input_State == INPUT_FOR_PASSWORD)
  {
    strcpy(password, Charactor_Array);
    EEPROM_write(PASSWORD_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End else if

    String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
    char charBuf[READ_EEPROM_MAX_SIZE] = {0};
    EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
    Wifi_User.setText(&charBuf[0]);

    EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
    EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
    Wifi_Password.setText(&charBuf[0]);
} //End function
//---------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Call Back For Page 6 ---------------------------------------------------//
void one3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '1';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void two3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '2';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void three3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '3';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void four3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '4';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void five3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '5';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void six3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '6';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void seven3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '7';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void eight3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '8';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void nine3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '9';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void zero3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '0';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void hash_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '#';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void ampersand_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '&';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

//Backslash cannot send via serial, I change to be @
void backslash_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '@';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void slash3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '/';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void underscore3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '_';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void minus_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '-';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void plus3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '+';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void left_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '(';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void right_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ')';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Sign3_Button_Callback(void *ptr) 
{
  page7.show();
  Current_Page = PAGE7;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(Charactor_Array);
} //End function

void star_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '*';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

//Double quote cannot send via serial, I change to $
void double_quote_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '$';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void single_quote_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = SINGLE_QUOTE;
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void dot_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '.';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void comma_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ',';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void exclamation3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '!';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void question_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '?';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Delete3_Button_Callback(void *ptr) 
{
  //Prevent minus index
  if(Charactor_Index > 0)
  {
    Charactor_Index--; //Delete alphabet before null
  }
  else
  {
    Charactor_Index = 0;
  }
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Back3_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for SSID
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_User.setText(&charBuf[0]);

  EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for password
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_Password.setText(&charBuf[0]);
} //End function

void Alphabet3_Button_Callback(void *ptr) 
{
  page5.show();
  Current_Page = PAGE5;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(Charactor_Array);
} //End function

void Space3_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ' ';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Next3_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  //Write data into eeprom
  String myString = String(Charactor_Array); //Convert string to char
  if(Input_State == INPUT_FOR_SSID)
  {
    strcpy(ssid, Charactor_Array);
    EEPROM_write(SSID_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End if
  else if(Input_State == INPUT_FOR_PASSWORD)
  {
    strcpy(password, Charactor_Array);
    EEPROM_write(PASSWORD_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End else if

    String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
    char charBuf[READ_EEPROM_MAX_SIZE] = {0};
    EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
    Wifi_User.setText(&charBuf[0]);

    EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
    EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
    Wifi_Password.setText(&charBuf[0]);
} //End function
//---------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------- Call Back For Page 7 ---------------------------------------------------//
void one4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '1';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void two4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '2';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void three4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '3';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void four4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '4';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void five4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '5';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void six4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '6';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void seven4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '7';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void eight4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '8';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void nine4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '9';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void zero4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '0';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void plus4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '+';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void multiply_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = 'x'; //This is not correct for multiply sign, I just put x
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void divide_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '/'; //This is not correct for divide sign, I just put slash
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void equal_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '=';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void triangle_right_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '>';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void soft_left_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '{';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void soft_right_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '}';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void strong_left_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '[';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void strong_right_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ']';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Sign4_Button_Callback(void *ptr) 
{
  page6.show();
  Current_Page = PAGE6;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(Charactor_Array);
} //End function

void colon_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ':';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void assign_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '@';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void triangle_left_bracket_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '<';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void dollar_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '$';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void percent_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '%';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void power_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = '^';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void semi_colon_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ';';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Delete4_Button_Callback(void *ptr) 
{
  //Prevent minus index
  if(Charactor_Index > 0)
  {
    Charactor_Index--; //Delete alphabet before null
  }
  else
  {
    Charactor_Index = 0;
  }
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Back4_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for SSID
  char charBuf[READ_EEPROM_MAX_SIZE] = {0};
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_User.setText(&charBuf[0]);

  EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM for password
  EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
  Wifi_Password.setText(&charBuf[0]);
} //End function

void Alphabet4_Button_Callback(void *ptr) 
{
  page5.show();
  Current_Page = PAGE5;
  delay(DELAY_PAGE_CHANGE);
  Input_Text2.setText(Charactor_Array);
} //End function

void Space4_Button_Callback(void *ptr) 
{
  Charactor_Array[Charactor_Index] = ' ';
  Charactor_Index++;
  Charactor_Array[Charactor_Index] = '\0'; //End of string
  Input_Text1.setText(Charactor_Array);
} //End function

void Next4_Button_Callback(void *ptr) 
{
  page2.show();
  Current_Page = PAGE2;
  delay(DELAY_PAGE_CHANGE);
  
  //Write data into eeprom
  String myString = String(Charactor_Array); //Convert string to char
  if(Input_State == INPUT_FOR_SSID)
  {
    strcpy(ssid, Charactor_Array);
    EEPROM_write(SSID_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End if
  else if(Input_State == INPUT_FOR_PASSWORD)
  {
    strcpy(password, Charactor_Array);
    EEPROM_write(PASSWORD_EEPROM_ADDR, myString);
    delay(DELAY_EEPROM);
  } //End else if

    String EEPROM_Temp = EEPROM_read(SSID_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
    char charBuf[READ_EEPROM_MAX_SIZE] = {0};
    EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
    Wifi_User.setText(&charBuf[0]);

    EEPROM_Temp = EEPROM_read(PASSWORD_EEPROM_ADDR, READ_EEPROM_MAX_SIZE); //Read EEPROM
    EEPROM_Temp.toCharArray(charBuf, READ_EEPROM_MAX_SIZE); //Convert string to char
    Wifi_Password.setText(&charBuf[0]);
} //End function

void AutoReconnect_CheckBox_Callback(void *ptr)
{
  uint32 Temp = EEPROM.read(AUTO_CONNECT_EEPROM_ADDR);
  if(Temp == NONE_AUTOCONNECT)
  {
    EEPROM.write(AUTO_CONNECT_EEPROM_ADDR, AUTOCONNECT);
    EEPROM.commit();
    delay(DELAY_EEPROM);
  }
  else if(Temp == AUTOCONNECT)
  {
    EEPROM.write(AUTO_CONNECT_EEPROM_ADDR, NONE_AUTOCONNECT);
    EEPROM.commit();
    delay(DELAY_EEPROM);
  }
  else //In case memory has another value, we have to write auto connect when we tick the box
  {
    EEPROM.write(AUTO_CONNECT_EEPROM_ADDR, AUTOCONNECT);
    EEPROM.commit();
    delay(DELAY_EEPROM);
  }
} //End function
//---------------------------------------------------------------------------------------------------------------------//

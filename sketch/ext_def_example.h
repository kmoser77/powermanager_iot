
static byte myip[] = { 192,168,0,123 };
static byte gwip[] = { 192,168,0,1 };
static byte dnsip[] = { 192,168,0,1 };
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte fhemIp[] = { 192,168,0,2 };

const char website[] PROGMEM = "server.fritz.box";
const char authHeader[] PROGMEM = "Authorization: Basic YWRtaW46cGFzc3dvcmQ=\r\n";

// Fhem dummies are PM01_IoT_Sw_01 .. PM01_IoT_Sw_07
const char devicePrefix[] PROGMEM = "PM01_IoT_Sw_0";


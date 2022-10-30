#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(10341)}, //XA_MainMenu
	{XA_Menu, XA_LENGTH(6687)},  //XA_GameOver
	{XA_Menu, XA_LENGTH(2742)}, //XA_Title
	{XA_Menu, XA_LENGTH(8602)},  //XA_Freeplay
	//WEEK1A.XA
	{XA_Week1A, XA_LENGTH(9726)}, //XA_Guy
	{XA_Week1A, XA_LENGTH(12872)}, //XA_Midnight
	//WEEK1B.XA
	{XA_Week1B, XA_LENGTH(21931)}, //XA_Terminated
	{XA_Week1B, XA_LENGTH(14400)}, //XA_Lurking
	//WEEK2A.XA
	{XA_Week2A, XA_LENGTH(17600)}, //XA_Lore
	{XA_Week2A, XA_LENGTH(8065)}, //XA_Blubber
	//WEEK2B.XA
	{XA_Week2B, XA_LENGTH(20367)}, //XA_Golden
	//WEEK3A.XA
	{XA_Week3A, XA_LENGTH(9750)},  //XA_Performance
	{XA_Week3A, XA_LENGTH(18285)}, //XA_Bite
	//WEEK3B.XA
	{XA_Week3B, XA_LENGTH(18666)}, //XA_Trapped
	//WEEK4A.XA
	{XA_Week4A, XA_LENGTH(10011)},  //XA_GoFish
	{XA_Week4A, XA_LENGTH(19141)}, //XA_Watchful
	//WEEK4B.XA
	{XA_Week4B, XA_LENGTH(12105)}, //XA_Restless
	{XA_Week4B, XA_LENGTH(1600)}, //XA_Beatbox
	//WEEK5A.XA
	{XA_Week5A, XA_LENGTH(11100)}, //XA_Showtime
	{XA_Week5A, XA_LENGTH(11927)},  //XA_Man
	//WEEK5B.XA
	{XA_Week5B, XA_LENGTH(21720)}, //XA_Followed
	//WEEK6A.XA
	{XA_Week6A, XA_LENGTH(17142)}, //XA_FazfuckNews
	{XA_Week6A, XA_LENGTH(14526)}, //XA_Criminal
	//WEEK6B.XA
	{XA_Week6B, XA_LENGTH(33359)}, //XA_MILLER
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\WEEK1A.XA;1", //XA_Week1A
	"\\MUSIC\\WEEK1B.XA;1", //XA_Week1B
	"\\MUSIC\\WEEK2A.XA;1", //XA_Week2A
	"\\MUSIC\\WEEK2B.XA;1", //XA_Week2B
	"\\MUSIC\\WEEK3A.XA;1", //XA_Week3A
	"\\MUSIC\\WEEK3B.XA;1", //XA_Week3B
	"\\MUSIC\\WEEK4A.XA;1", //XA_Week4A
	"\\MUSIC\\WEEK4B.XA;1", //XA_Week4B
	"\\MUSIC\\WEEK5A.XA;1", //XA_Week5A
	"\\MUSIC\\WEEK5B.XA;1", //XA_Week5B
	"\\MUSIC\\WEEK6A.XA;1", //XA_Week6A
	"\\MUSIC\\WEEK6B.XA;1", //XA_Week6B
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"mainmenu", false},   //XA_MainMenu
	{"gameover", false}, //XA_GameOver
	{"title", false},   //XA_Title
	{"freeplay", false},   //XA_Freeplay
	//WEEK1A.XA
	{"guy", true}, //XA_Guy
	{"midnight", true},   //XA_Midnight
	//WEEK1B.XA
	{"terminated", true}, //XA_Terminated
	{"lurking", true}, //XA_Lurking
	//WEEK2A.XA
	{"lore", true}, //XA_Lore
	{"blubber", true},    //XA_Blubber
	//WEEK2B.XA
	{"golden", true}, //XA_Golden
	//WEEK3A.XA
	{"performance", true},   //XA_Performance
	{"bite", true}, //XA_Bite
	//WEEK3B.XA
	{"trapped", true}, //XA_Trapped
	//WEEK4A.XA
	{"gofish", true}, //XA_GoFish
	{"watchful", true},         //XA_Watchful
	//WEEK4B.XA
	{"restless", true}, //XA_Restless
	{"beatbox", true}, //XA_Beatbox
	//WEEK5A.XA
	{"showtime", true},  //XA_Showtime
	{"man", true}, //XA_Man
	//WEEK5B.XA
	{"followed", true}, //XA_Followed
	//WEEK6A.XA
	{"fazfucknews", true}, //XA_FazfuckNews
	{"criminal", true},  //XA_Criminal
	//WEEK6B.XA
	{"miller", true}, //XA_MILLER
	
	{NULL, false}
};

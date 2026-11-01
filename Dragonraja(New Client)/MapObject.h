//#pragma once
//
//#include "Sprite.h"
//#include"GameProc.h"
//#define ITEM_FRAME_MAX_ 10
//#define MAX_MAPOBJECTIMAGE			2000
//#define MAX_MAPOBJECT_				5000
//#define Random( x )				(::rand() % ( (0 < (x) )? (x) : (1) ))
//#define OBJECTANI_NO				1
//#define OBJECTANI_RETURNLOOP		2	
//#define OBJECTANI_LOOP				3
//#define OBJECTANI_1_RETURNLOOP		4
//#define OBJECTANI_1_LOOP			5
//#define OBJECTANI_TOGGLE_ANI		6
//
//#define OBJECTSPRITE_NORMAL			0
//#define OBJECTSPRITE_TRANS			1
//#define OBJECTSPRITE_ALPHA     		2
//
//
//
//#define	SPRITETYPE_NONE				 0		// 마우스에 의해 CHARACTER선택되는 루틴에서 사용
//#define	SPRITETYPE_CHARACTER		 1 		// 출력.. Sortting.. 
//#define	SPRITETYPE_NPC				 2
//#define	SPRITETYPE_MONSTER			 3
//#define	SPRITETYPE_OBJECT			 4
//#define	SPRITETYPE_OBJECT_BOTTOM	 5  
//#define	SPRITETYPE_TILE				 6
//#define SPRITETYPE_ITEM				 7
//#define SPRITETYPE_ITEM_BOTTOM		 8
//#define SPRITETYPE_EFFECT_NORMAL	 9
//#define SPRITETYPE_EFFECT_CEILING		10
//#define SPRITETYPE_EFFECT_BOTTOM		11
//#define SPRITETYPE_NUMBER				12
//#define SPRITETYPE_FOOT					13
//#define SPRITETYPE_DEST_POINT			14	
//#define SPRITETYPE_DEST_POINT_BOTTOM	15
//#define SPRITETYPE_SKILLCURSOR			16
//#define SPRITETYPE_DETECT_TILE			17
//#define SPRITETYPE_SKILLGAUGE			18
//#define	SPRITETYPE_CLOCKNIDDLE			19	
//#define SPRITETYPE_ON_THE_CEILING_CHAR	20
//#define SPRITETYPE_ALPHAOBJECT			21	
//
//
//
//#define	OBJECTATTR_BACK_ABOVE		1
//#define	OBJECTATTR_BACK_BELOW		2
//#define	OBJECTATTR_FRONT_ABOVE		3
//#define	OBJECTATTR_FRONT_BELOW		4
//#define	OBJECTATTR_ROOF_ABOVE		5
//#define	OBJECTATTR_ROOF_BELOW		6
//
//#define	OBJECTDIRECTION_RIGHTUP		1
//#define OBJECTDIRECTION_RIGHTDOWN	2
//#define	OBJECTDIRECTION_LEFTDOWN	3
//#define	OBJECTDIRECTION_LEFTUP		4
//
//#define	OBJECTANIMATION_STOP		1
//#define	OBJECTANIMATION_LOOP		2
//#define	OBJECTANIMATION_PLAY		3
//
//
//#define OBJECTANI_NO				1
//#define OBJECTANI_RETURNLOOP		2	
//#define OBJECTANI_LOOP				3
//#define OBJECTANI_1_RETURNLOOP		4
//#define OBJECTANI_1_LOOP			5
//#define OBJECTANI_TOGGLE_ANI		6
//
//
//#define OBJECTSPRITE_NORMAL			0
//#define OBJECTSPRITE_TRANS			1
//#define OBJECTSPRITE_ALPHA          2
//enum ITEMTYPE_INDEX
//{
//	ITEMTYPE_NORMAL = 0,
//	ITEMTYPE_CHAIR = 1,
//	ITEMTYPE_TABLE = 2,
//	ITEMTYPE_DOOR = 3,
//	ITEMTYPE_BOX = 4,
//	ITEMTYPE_SIGNBOARD = 5,
//	ITEMTYPE_DIRECTBOARD = 6,
//};
//enum ITEMANIMATION_TYPE
//{
//	ITEMANIMATION_NO = 0,
//	ITEMANIMATION_REPEAT = 1,
//	ITEMANIMATION_RETURN = 2,
//	ITEMANIMATION_OPENNING = 3,
//	ITEMANIMATION_OPENED = 4,
//	ITEMANIMATION_CLOSING = 5,
//	ITEMANIMATION_CLOSED = 6,
//	ITEMANIMATION_FARM0 = 7,
//	ITEMANIMATION_FARM1 = 8,
//	ITEMANIMATION_FARM2 = 9,
//	ITEMANIMATION_FARM3 = 10,
//	ITEMANIMATION_FARM4 = 11,
//	ITEMANIMATION_FARM5 = 12,
//	ITEMANIMATION_FARM6 = 13,
//	ITEMANIMATION_FARM7 = 14,
//	ITEMANIMATION_FIRE = 15,
//	ITEMANIMATION_ACTIVE = 16,
//};
//enum ITEM_STRUCT_INDEX
//{
//	IATTR_DURATION = 0,
//	IATTR_LIMIT = 1,
//	IATTR_ATTR = 2,
//	IATTR_ID = 3,
//	IATTR_RESERVED0 = 4,
//	IATTR_RESERVED1 = 5,
//};
//enum IA2_TYPE
//{
//	IA2_HIDE = 0x00000001,
//	IA2_NOTMOVE = 0x00000002,
//	IA2_POISONED = 0x00000004,
//	IA2_OPENED = 0x00000008,
//	IA2_BROKEN = 0x00000010,
//	IA2_HOUSE = 0x00000020,
//	IA2_MAGICED = 0x00000040,
//	IA2_CANNOTUSE = 0x00000080,
//	IA2_NOIDENTIFIED = 0x00000100,
//	IA2_DOOR = 0x00000200,
//	IA2_BOX = 0x00000400,
//	IA2_CHAIR = 0x00000800,
//	IA2_TABLE = 0x00001000,
//	IA2_FARM0 = 0x00002000,
//	IA2_FARM1 = 0x00004000,
//	IA2_FARM2 = 0x00008000,
//	IA2_FARM3 = 0x00010000,
//	IA2_FARM4 = 0x00020000,
//	IA2_FARM5 = 0x00040000,
//	IA2_FARM6 = 0x00080000,
//	IA2_FARM7 = 0x00100000,
//	IA2_FIRE = 0x00200000,
//	IA2_TRAPED0 = 0x00400000,
//	IA2_TRAPED1 = 0x00800000,
//	IA2_TRAPED2 = 0x01000000,
//	//	IA2_FIRST_ITEM		= 0x02000000,
//	IA2_SEAL_NOR = 0x04000000,
//	IA2_SEAL_SPC = 0x08000000,
//	IA2_COLOSSUS_STONE = 0x10000000,
//	IA2_SCENARIO_POTAL = 0x20000000,
//};
//typedef struct
//{
//	__int16 nLevel;
//	DWORD	nExp;
//	DWORD	nExpNext;
//
//	//	char	sCharacterName[MAX_NAME];
//		//char	nGuild[MAX_NAME];
//
//		//int		nCharacterData[MAX_CHARACTERDATA];  // 성별,얼굴,가치관,클래스,스펠,머리색,피부색,옷색,장식색,택틱스남,택틱스여,직업
//	int		nCharacterAbility[20];				//STR,CON,DEX,WIS,INT,CHA,MOVP,ENDU,MOR,CONV,LUCK, WSPS,
//	int		nAbilityMax[20];					//STR,CON,DEX,WIS,INT,CHA,MOVP,ENDU,MOR,CONV,LUCK, WSPS,
//
//	DWORD	nMoney;		// 돈
//
//	int		nCharacterFace;						// 캐릭터 얼굴 표정을 저장 총8가지가 있다
//	int		nCharacterHP;						// HP와 MP를 저장하는 변수
//	int		nCharacterMP;
//	int		nCharacterMAXHP;					// HP와 MP의 최대치
//	int		nCharacterMAXMP;
//	int		nCharacterSP;
//	int		nCharacterMAXSP;
//
//	BYTE	condition;		// 캐릭터의 상태를 저장하는 변수 0이면 정상이다. 
//
//	int		nWeight;					// 들고 다니는 아이템의 무게
//	DWORD	BankMoney;					//	
//	int		LoanTotal;
//	int		LastLoan;
//	DWORD	LastLoan_Time;
//
//	//	int		nJobSkill_Lv[31];				// 캐러의 직업스킬 능력치
//	//	DWORD	nJobSkill[MAX_MOTHER_SKILL];					// 기술 경험치
//	//t_SkillExp	nJobSkill[MAX_MOTHER_SKILL];					// 기술 경험치//011013 lsw 
//	//	int		nJopSkillHit[31];				// 성공률
//
//	//BYTE	MagicId[MAX_MAGIC];					// 사용할 수 있는 매직
//	//BYTE	SkillId[MAX_SKILL_ID];					// 할수 있는 스킬
//	//short int	ItemId[MAX_LEARN_ITEM];			// 만들수 있는 아이템
//	int		ItemIdCount;					// 내가 만들수 있는 아이템 수...
//
//	//int		curr_magic[MEMORIZE_MAX];		// 현재 메모라이즈 한 마법
//	int		curr_magic_ct;					//						   의 총수
//
//	int		memorize_point;					// 현재 메모라이즈 하고 남은 포인트
//	int		memorize_point_max;				// 메모라이즈 할 수 있는 포인트 최대치	// 현재 wsps+3
//
//
//	//	int		TacticsLv[13];					// 택틱스 레벨
//	DWORD	TacticsExp[13];					// 택틱스 경험치
//	DWORD	TacticsExpNext[13];
//
//	//short naBasicResist[MAX_RESIST_TYPE];
//	//short naExtentionResist[MAX_RESIST_TYPE];
//
//	int		LvUpPoint;
//	WORD 	EachUp[11];
//
//	int		body_r;			//  외모 색
//	int		body_g;
//	int		body_b;
//	int		cloth_r;
//	int		cloth_g;
//	int		cloth_b;
//
//	int		mantle_r; // finito 23/06/08 - mantle dye
//	int		mantle_g;
//	int		mantle_b;
//
//	int		nk3, nk4, nk6;		// 국가 적대감
//
//	int		age;			// 나이
//
//	int		tatics;			// 현재 착용 아이템의 텍틱스
//	int		beat;			// 내가 죽인 몬스터의 수
//	int		ac;				// 방어력
//	int		mov;			// 스테이터스 창에 나타나는 이동력
//
//	int		accessory[4];	// 현재 착용하고 있는 갑옷, 무기 등등...
//
//	BYTE	believe_god;			// 섬기는 신 0: 없음... ~
//	int		faith;					// 신앙심
//	char	much_supper;			// 신전에서 음식 제공할때 그 갯수
//
//	int		dmg_count;		// first	// 3d2 ( 1~2인 주사위를 3번 던진다.)
//	int		dmg_max;		// second
//
//	//	BYTE	dmg_count_of_inv[8];
//	//	BYTE	dmg_max_of_inv[8];		// 굳이 잡을 필요없이 타격 계산할때 그때마다 사용한다.
//
//	bool	disease[6];			// 질병에 걸렸는지...
//	DWORD	disease_no_time[6];
//
//	DWORD	time_maintain1;				// 저항 효과 지속 시간
//	DWORD	time_maintain2;				// 능력 효과 지속 시간
//	DWORD	time_maintain3;				// 상태 변환 효과 지속 시간
//
//	//CharacterParty	party[6];			// 맺은 인물의 이름
//	//CharacterParty	relation[3];
//	//CharacterParty	employment[6];
//
//	int		nAbilityPlusOfPotion[20];	// 먹는 거에 의한 파워업 ( 겹치기는 안 된다. )
//	DWORD	dwAbilityPluseTime[20];		// 먹는 거에 의한 파워업일 경우 그 유지 시간.
//	int		nAbilityPlusOfWeapon[20];	// 아이템 장착에 의한 파워업 ( 겹치기 가능 )
//	// 매직 부분 필요
//
//	BYTE	magic_casting_time[200];		// 단위:초*10
//
//	int		sleep;			// 자고 일어 났거나 새로 시작했을 경우...
//
//	int		class_famous;
//	int		job_famous;
//	int		nation;
//
//	//char aStepInfo[20];
//	int m_nQuestStep;	// 스파이 퀘스트
//	int m_nSelectRune;
//	int potion_box1;		// 포션박스
//
//	// 020428 YGI acer
//	DWORD attack_time;
//
//}SCharacter;
//typedef struct tagBOARDDATA
//{
//	WORD	boardNumber;
//	char	str_boardData[80];
//}	BOARDDATA;
//#define MAX_DSD_STRING	260
//struct DIRECTBOARDDATA
//{	//< CSD-030324			
//	int		boardNumber;
//	char	str_Title[MAX_DSD_STRING];
//	char	str_East[MAX_DSD_STRING];
//	char	str_West[MAX_DSD_STRING];
//	char	str_South[MAX_DSD_STRING];
//	char	str_North[MAX_DSD_STRING];
//};	//> CS
//const int IATTR_MUCH = IATTR_DURATION;
//const int IATTR_SEAL_STONE_NO = IATTR_LIMIT;
//const int IATTR_SPRITE_TYPE = IATTR_LIMIT;
//const int IATTR_RARE_MAIN = IATTR_ID;
//const int IATTR_TREASURE_MAP = IATTR_RESERVED0;	// 040105 YGI
//enum ITEMTYPE_WORKTABLE_TYPE
//{
//	ITEMTYPE_WORKTABLE_MINING = 71,	// 작업대 용광로(71)
//	ITEMTYPE_WORKTABLE_BATTLE = 72,	// 작업대 배틀
//	ITEMTYPE_WORKTABLE_SUNBAN = 73,  // 작업대 선반
//	ITEMTYPE_WORKTABLE_MORU = 74,	// 작업대 모루
//	ITEMTYPE_WORKTABLE_HWADUK = 75,	// 작업대 화덕
//	ITEMTYPE_WORKTABLE_SIHUMKWAN = 76,  // 작업대 시험관류
//	ITEMTYPE_WORKTABLE_MULAE = 77,	// 작업대 물레
//	ITEMTYPE_WORKTABLE_BULDAE = 78,// 작업대 불대 
//	ITEMTYPE_SEAL_NOR = 79,
//	ITEMTYPE_SEAL_SPC = 80,
//	ITEMTYPE_HP_UP = 81,
//	ITEMTYPE_MP_UP = 82,
//	ITEMTYPE_SCALE = 83,
//	ITEMTYPE_NATION_STONE = 84,
//	ITEMTYPE_GUILD_STONE = 85,
//	ITEMTYPE_SENARIO_STONE = 86,
//	ITEMTYPE_COLOSSUS_STONE = 87,
//};
//#pragma pack(push,1)
//typedef struct tagMAPOBJECT_old {
//	WORD  id;
//	BYTE  status;
//	WORD  objectoritem;
//	char  curframe;
//	WORD  dum;
//	BYTE  animationType;
//	int   x, y;
//	BYTE  attr;
//	BYTE  attr1;
//	BYTE  attr2;
//	WORD  dir;
//	int   offx, offy;
//	short dx0[ITEM_FRAME_MAX_], dy0[ITEM_FRAME_MAX_];
//	short dx1[ITEM_FRAME_MAX_], dy1[ITEM_FRAME_MAX_];
//
//	BYTE  light;
//	short lightx, lighty;
//	char  delay;
//	char  anidir;
//
//}MAPOBJECT_old, * LPMAPOBJECT_old;
//typedef struct tagMAPITEM
//{
//	int nFrameCount;
//	Spr	 spr[ITEM_FRAME_MAX_];
//}MAPOBJECTIMAGE, * LPMAPOBJECTIMAGE;
//
//typedef struct tagMAPOBJECT {
//	WORD  id;
//	BYTE  status;
//	WORD  objectoritem;
//	char  curframe;
//	WORD  dum;
//	BYTE  animationType;
//	int   x, y;
//	BYTE  attr;
//	BYTE  attr1;
//	BYTE  attr2;
//	WORD  dir;
//	int   offx, offy;
//	short dx0[ITEM_FRAME_MAX_], dy0[ITEM_FRAME_MAX_];
//	short dx1[ITEM_FRAME_MAX_], dy1[ITEM_FRAME_MAX_];
//
//	BYTE  light;
//	short lightx, lighty;
//	char  delay;
//	char  anidir;
//
//	////////////////// SoundUp lkh 추가 /////////////////
//	unsigned short soundno;
//	unsigned short soundframe;
//	unsigned short sounddelay;
//
//	// 0109 ItemDontBox													
//	//	short dtx0, dty0;												
//	//	short dtx1, dty1;										
//	//	short dtx2, dty2;											
//	//	short dtx3, dty3;											
//
//}MAPOBJECT, * LPMAPOBJECT;
//typedef struct	tagOBJECTATTR
//{
//	WORD	wID;				//	saf화일명..
//	WORD	wAnimationCount;	//  
//	WORD	wAnimationOrder;	//
//	BYTE	cAnimationType;		//  정지 .. Animation...
//	BYTE	cAttr;
//	BYTE	cAttr1;
//	BYTE	cAttr2;
//	WORD	wCurrentFrame;		//
//	WORD	wDirection;
//	POINT	position;			// 타일의 위치.
//	POINT	offset;				// 타일에서의 변이값..
//	POINT	origin;				//  // 현재는 사용 않함..
//} OBJECTATTR, * LPOBJECTATTR;
//typedef struct	tagOBJECT
//{
//	OBJECTATTR			ObjectAttr;
//
//	struct tagOBJECT* lpPrev, * lpNext;
//} OBJECT, * LPOBJECT;
//typedef struct sItem
//{
//	int server_id;
//	int no;
//	unsigned int  attr[6];
//	short status;
//	short sx, sy;
//	short offx, offy;
//	short x, y;
//	short mx, my;
//	Spr* spr[10];
//	short  dsx[10], dsy[10], ddx[10], ddy[10];
//
//	DWORD	host;
//	BYTE	type;
//	BYTE	getable;
//	BYTE	Anitype;
//	char	Sorttype;
//	BYTE	Where;
//	BYTE	shadow;
//	char	framecount;
//	char	dirno;
//	short	dx[4], dy[4];
//
//	short  curframe;
//	char   anitoggle;
//
//	struct sItem* prev;
//	struct sItem* next;
//}ITEMGROUND, * LPITEMGROUND;
//extern int LoadTOI( const char* filename);
//extern MAPOBJECTIMAGE      MoImage[MAX_MAPOBJECTIMAGE * 10];
//extern MAPOBJECT			Mo[MAX_MAPOBJECT_];;
//extern uint8_t TotalMapObject;;
//extern ORDERINFO		g_OrderInfo;
//
//extern void DisplayMapObject(LPMAPOBJECT mo);
//extern int					Mapx, Mapy;
//extern int CrashOiOj(LPORDERTABLE oi, LPORDERTABLE oj);
//#pragma pack(pop)
#ifndef EV_CHARGEPOINT_PROTOCOL_H_
#define EV_CHARGEPOINT_PROTOCOL_H_

typedef enum ePage {
	PAGE_BEGIN = 101,                                    
	PAGE_SCREEN_SAVER = 101, 	 
	PAGE_CHOOSE_LANGUAGE,    	 
	PAGE_AUTHENTICATE,		 	 
	PAGE_PLUGIN,			 	 
	PAGE_TRANSACTION,		 	 
	PAGE_CHARGING,			 	 
	PAGE_REPORT,			 	 
	PAGE_MAX											 
} E_PAGE;

typedef enum eLang {
	LANG_EN = PAGE_MAX,
	LANG_DE,
	LANG_CN,
	LANG_MAX
} E_LANG;

#define SYNC_CHARGE_PERCENT(x) (x >= 0 && x <= 100? x:0 )
#endif

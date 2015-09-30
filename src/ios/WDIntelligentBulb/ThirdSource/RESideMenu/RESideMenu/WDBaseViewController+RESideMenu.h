//
//  WDBaseViewController+RESideMenu.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/21.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDBaseViewController.h"

@class RESideMenu;

@interface WDBaseViewController (RESideMenu)

@property (strong, readonly, nonatomic) RESideMenu *sideMenuViewController;

// IB Action Helper methods

- (IBAction)presentLeftMenuViewController:(id)sender;
- (IBAction)presentRightMenuViewController:(id)sender;

@end

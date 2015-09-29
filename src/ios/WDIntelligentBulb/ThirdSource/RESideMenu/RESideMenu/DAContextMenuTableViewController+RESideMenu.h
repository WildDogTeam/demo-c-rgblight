//
//  DAContextMenuTableViewController+RESideMenu.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/20.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "DAContextMenuTableViewController.h"

@class RESideMenu;

@interface DAContextMenuTableViewController (RESideMenu)

@property (strong, readonly, nonatomic) RESideMenu *sideMenuViewController;

// IB Action Helper methods

- (IBAction)presentLeftMenuViewController:(id)sender;
- (IBAction)presentRightMenuViewController:(id)sender;

@end

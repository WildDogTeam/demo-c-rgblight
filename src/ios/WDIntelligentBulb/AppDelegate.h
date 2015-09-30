//
//  AppDelegate.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <Wilddog/Wilddog.h>
#import "RESideMenu.h"

#define ApplicationDelegate ((AppDelegate *)[UIApplication sharedApplication].delegate)

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) Wilddog *wilddog;

@property (strong, nonatomic) RESideMenu *sideMenuTabBarViewController;

@end


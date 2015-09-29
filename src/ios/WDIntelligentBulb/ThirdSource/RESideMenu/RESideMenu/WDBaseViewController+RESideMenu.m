//
//  WDBaseViewController+RESideMenu.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/21.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDBaseViewController+RESideMenu.h"

#import "RESideMenu.h"

@implementation WDBaseViewController (RESideMenu)



- (RESideMenu *)sideMenuViewController
{
    UIViewController *iter = self.parentViewController;
    while (iter) {
        if ([iter isKindOfClass:[RESideMenu class]]) {
            return (RESideMenu *)iter;
        } else if (iter.parentViewController && iter.parentViewController != iter) {
            iter = iter.parentViewController;
        } else {
            iter = nil;
        }
    }
    return nil;
}

#pragma mark -
#pragma mark IB Action Helper methods

- (IBAction)presentLeftMenuViewController:(id)sender
{
    [self.sideMenuViewController presentLeftMenuViewController];
}

- (IBAction)presentRightMenuViewController:(id)sender
{
    [self.sideMenuViewController presentRightMenuViewController];
}


@end

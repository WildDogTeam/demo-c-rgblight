//
//  WDDeviceManagerViewController.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "DAContextMenuTableViewController.h"

@interface WDDeviceManagerViewController : DAContextMenuTableViewController

-(void)pushSearchViewController;

-(void)refreshDeviceListWith:(NSMutableArray *)deviceDataArr;


@end

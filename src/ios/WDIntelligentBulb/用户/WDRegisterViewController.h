//
//  WDRegisterViewController.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDBaseViewController.h"
#import "RegisterProtol.h"


@interface WDRegisterViewController : WDBaseViewController

@property(nonatomic, weak)id<RegisterProtol> delegate;

@end

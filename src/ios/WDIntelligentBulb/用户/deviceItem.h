//
//  deviceItem.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/20.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface deviceItem : NSObject

@property(nonatomic, strong)NSString *deviceID;
@property(nonatomic, strong)NSString *deviceNickName;
@property(nonatomic, strong)NSString *deviceRed;
@property(nonatomic, strong)NSString *deviceBlue;
@property(nonatomic, strong)NSString *deviceGreen;
@property(nonatomic, strong)NSString *deviceSW;

-(NSString *)showDeviceName;

@end

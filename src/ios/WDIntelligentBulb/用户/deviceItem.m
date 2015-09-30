//
//  deviceItem.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/20.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "deviceItem.h"

@implementation deviceItem

-(id)init
{
    if (self = [super init]) {
        
        self.deviceID = @"";
        self.deviceNickName = @"";
        self.deviceRed = @"";
        self.deviceBlue = @"";
        self.deviceGreen = @"";
        self.deviceSW = @"";
        
    }
    
    return self;
}

-(NSString *)showDeviceName
{
    
    NSLog(@"showDeviceName: %@   %@", _deviceID, _deviceNickName);
    
    return _deviceNickName == nil? _deviceID : _deviceNickName;
    
}

@end

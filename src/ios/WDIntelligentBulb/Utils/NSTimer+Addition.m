//
//  NSTimer+Addition.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/15.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "NSTimer+Addition.h"

@implementation NSTimer (Addition)

-(void)pauseTimer{
    
    if (![self isValid]) {
        return ;
    }
    
    [self setFireDate:[NSDate distantFuture]];

}


-(void)resumeTimer{
    
    if (![self isValid]) {
        return ;
    }
    
    [self setFireDate:[NSDate date]];
    
}

@end

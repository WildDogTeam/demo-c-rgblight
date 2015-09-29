//
//  RegisterProtol.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/24.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol RegisterProtol <NSObject>

@optional
-(void)RegisterSuccessWithAccount:(NSString *)accout andPassword:(NSString *)Password;

@end

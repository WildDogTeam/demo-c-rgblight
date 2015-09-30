//
//  Utils.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/10.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>


@interface Utils : NSObject


+(BOOL)isEmptyWithStr:(NSString *)str;

//网络状态指示器
+(void)showProgressHUdnView:(UIView *)view;

+(void)showProgressHUdWithText:(NSString*)text showInView:(UIView *)view;

+(void)hidenProgressHUdshowInView:(UIView *)view;

+(void)showStatusTextWithText:(NSString *)text showInView:(UIView *)view;

//正则表达式
+(BOOL)isValidateEmail:(NSString *)email;

+(NSDictionary *)toJsonWithDict:(NSDictionary *)dictParam;

+ (NSData *)toJSONData:(id)theData;

//记住上次登录用户名
+ (void)saveLastLoginAccount:(NSString *)name;

+ (NSString *)readLastLoginAccount;


@end

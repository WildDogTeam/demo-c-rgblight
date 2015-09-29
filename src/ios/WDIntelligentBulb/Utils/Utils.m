//
//  Utils.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/10.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "Utils.h"

#import "MBProgressHUD.h"

@implementation Utils




//将字典转化为json串
+(NSDictionary *)toJsonWithDict:(NSDictionary *)dictParam
{
    
    NSString *jsonString = [[NSString alloc] initWithData:[self toJSONData:dictParam] encoding:NSUTF8StringEncoding];
    
    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:jsonString,@"json", nil];
    
    
    
    return dict;
}


// 将字典或者数组转化为JSON串
+ (NSData *)toJSONData:(id)theData{
    
    NSError *error = nil;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:theData
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:&error];
    
    if ([jsonData length] > 0 && error == nil){
        return jsonData;
    }else{
        return nil;
    }
}

+(BOOL)isEmptyWithStr:(NSString *)str
{
    
    
    return [str isEqualToString:@""];
}

+(void)showProgressHUdnView:(UIView *)view
{
    MBProgressHUD *hud=[MBProgressHUD showHUDAddedTo:view animated:YES];
    hud.mode=MBProgressHUDModeIndeterminate;
    hud.removeFromSuperViewOnHide=YES;
    [hud show:YES];
}


+(void)showProgressHUdWithText:(NSString*)text showInView:(UIView *)view
{
    MBProgressHUD *hud=[MBProgressHUD showHUDAddedTo:view animated:YES];
    hud.mode=MBProgressHUDModeIndeterminate;
    hud.removeFromSuperViewOnHide=YES;
    [hud setLabelText:text];
    [hud show:YES];
    
}


+(void)hidenProgressHUdshowInView:(UIView *)view
{
    [MBProgressHUD hideHUDForView:view animated:YES];
}

+(void)showStatusTextWithText:(NSString *)text showInView:(UIView *)view
{
    //显示状态
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:view animated:YES];
    
    // Configure for text only and offset down
    hud.mode = MBProgressHUDModeText;
    hud.labelText = text;
    hud.margin = 20.f;
    hud.yOffset = -20.f;
    hud.removeFromSuperViewOnHide = YES;
    
    [hud hide:YES afterDelay:1];
}


//利用正则表达式验证
+(BOOL)isValidateEmail:(NSString *)email {
    NSString *emailRegex = @"[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}";
    NSPredicate *emailTest = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", emailRegex];
    return [emailTest evaluateWithObject:email];
}

+ (void)saveLastLoginAccount:(NSString *)name
{
    if (name == nil) {
        name = @"";
    }
    [[NSUserDefaults standardUserDefaults]setObject:name forKey:@"LastLoginAccount"];
    [[NSUserDefaults standardUserDefaults]synchronize];
}

+ (NSString *)readLastLoginAccount
{
    return [[NSUserDefaults standardUserDefaults]objectForKey:@"LastLoginAccount"] == nil ? @"" :[[NSUserDefaults standardUserDefaults]objectForKey:@"LastLoginAccount"];
}

@end














































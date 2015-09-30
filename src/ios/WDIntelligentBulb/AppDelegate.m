//
//  AppDelegate.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "AppDelegate.h"
#import <TencentOpenAPI/TencentOAuth.h>
#import "WXApi.h"
#import "WeiboSDK.h"

#import "Utils.h"
#import "WDLoginViewController.h"
#import "SideMenuViewController.h"
#import "RESideMenu.h"
#import "deviceItem.h"
#import "WDDeviceSearchViewController.h"
#import "WDDeviceManagerViewController.h"
#import "UIColor+Util.h"

//写入自己的APP域名,下面的知识例子
#define   APP_URL       @"https://zhinengcaideng.wilddogio.com"

@interface AppDelegate ()<WXApiDelegate,WeiboSDKDelegate>
{
    UINavigationController *_navContent;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    [UIApplication sharedApplication].statusBarStyle = UIStatusBarStyleLightContent;
    [[UINavigationBar appearance] setTitleTextAttributes:@{NSForegroundColorAttributeName:[UIColor whiteColor]}];
    [UINavigationBar appearance].tintColor = [UIColor whiteColor];

    
    //向微信注册
    [WXApi registerApp:WX_KEY];
    
    //向微博注册
    [WeiboSDK registerApp:WB_KEY];
    [WeiboSDK enableDebugMode:YES];
    
    
    
    self.wilddog = [[Wilddog alloc] initWithUrl:APP_URL];
    
    
    
    WDLoginViewController *loginViewController = [[WDLoginViewController alloc] initWithNibName:@"WDLoginViewController" bundle:nil];
    _navContent = [[UINavigationController alloc] initWithRootViewController:loginViewController];
    
    
    
    _sideMenuTabBarViewController = [[RESideMenu alloc] initWithContentViewController:_navContent
                                                               leftMenuViewController:[SideMenuViewController new]
                                                              rightMenuViewController:nil];
    _sideMenuTabBarViewController.scaleContentView = YES;
    _sideMenuTabBarViewController.contentViewScaleValue = 0.95;
    _sideMenuTabBarViewController.scaleMenuView = NO;
    _sideMenuTabBarViewController.contentViewShadowEnabled = YES;
    _sideMenuTabBarViewController.contentViewShadowRadius = 4.5;
    
    self.window.rootViewController = _sideMenuTabBarViewController;
    
    return YES;
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
    if ([url.absoluteString hasPrefix:@"wx"]) {
        return [WXApi handleOpenURL:url delegate:self];
    } else if ([url.absoluteString hasPrefix:@"tencent"]) {
        return [TencentOAuth HandleOpenURL:url];
    }else if ([url.absoluteString hasPrefix:@"wb"]) {
        return [WeiboSDK handleOpenURL:url delegate:self ];
    }
    return nil;
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    if ([url.absoluteString hasPrefix:@"wx"]) {
        return [WXApi handleOpenURL:url delegate:self];
    } else if ([url.absoluteString hasPrefix:@"tencent"]) {
        return [TencentOAuth HandleOpenURL:url];
    }else if ([url.absoluteString hasPrefix:@"wb"]) {
        return [WeiboSDK handleOpenURL:url delegate:self ];
    }
    return nil;
}

#pragma mark - WeiChatSDKDelegate
-(void) onReq:(BaseReq*)req
{
    
}
/*! @brief 发送一个sendReq后，收到微信的回应
 *
 * 收到一个来自微信的处理结果。调用一次sendReq后会收到onResp。
 * 可能收到的处理结果有SendMessageToWXResp、SendAuthResp等。
 * @param resp具体的回应内容，是自动释放的
 */
-(void) onResp:(BaseResp*)resp
{
    if([resp isKindOfClass:[SendAuthResp class]])
    {
        SendAuthResp *response = (SendAuthResp*)resp;
        if(response.code.length == 0){
            return;
        }
        [ApplicationDelegate.wilddog authWithOAuthProvider:@"weixin" parameters:@{@"code":response.code} withCompletionBlock:^(NSError *error, WAuthData *authData) {
            
            [Utils hidenProgressHUdshowInView:self.window];
            
            if (authData) {
                
                
                [UserDefaults setObject:authData.uid forKey:UID];
                [UserDefaults setObject:authData.token forKey:TOKEN];
                [UserDefaults synchronize];
                
                
                NSLog(@"authData.uid : %@", authData.uid );
                

                
                Wilddog *manageWilddog = [ApplicationDelegate.wilddog childByAppendingPath:@"devicemanage"];
                Wilddog *uidWilddog = [manageWilddog childByAppendingPath:authData.uid];
                
                [uidWilddog observeSingleEventOfType:WEventTypeValue withBlock:^(WDataSnapshot *snapshot) {
                    
                    
                    [Utils showStatusTextWithText:@"登录成功" showInView:self.window];
                    
                    NSInteger deviceCount = [snapshot.value count];
                    
                    NSDictionary *deviceDict = [[NSDictionary alloc] initWithDictionary:snapshot.value];

                    NSMutableArray *deviceArr = [[NSMutableArray alloc] init];
                    [deviceDict enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {

                        
                        deviceItem *item = [[deviceItem alloc] init];
                        item.deviceID = key;
                        item.deviceBlue = [[obj objectForKey:@"led"] objectForKey:@"blue"];
                        item.deviceRed = [[obj objectForKey:@"led"] objectForKey:@"red"];
                        item.deviceGreen = [[obj objectForKey:@"led"] objectForKey:@"green"];
                        item.deviceSW = [[obj objectForKey:@"led"] objectForKey:@"sw"];
                        item.deviceNickName = [UserDefaults objectForKey:[NSString stringWithFormat:@"%ld%@",[item.deviceID hash], item.deviceID]];
                        
                        [deviceArr addObject:item];
                        
                    }];
                    
                    /*
                     没有绑定任何设备则为进入设备搜索页
                     有绑定设备进入设备管理页
                     */
                    if (0 == deviceCount) {
                        
                        WDDeviceManagerViewController *deviceManagerVC = [[WDDeviceManagerViewController alloc] initWithNibName:@"WDDeviceManagerViewController" bundle:nil];
                        [_navContent pushViewController:deviceManagerVC animated:NO];
                        
                        [deviceManagerVC pushSearchViewController];
                        
                    }else {
                        
                        WDDeviceManagerViewController *deviceManagerVC = [[WDDeviceManagerViewController alloc] initWithNibName:@"WDDeviceManagerViewController" bundle:nil];
                        [deviceManagerVC refreshDeviceListWith:deviceArr];
                        [_navContent.topViewController.navigationController pushViewController:deviceManagerVC animated:YES];
                        
                    }
                    
                    NSLog(@"authData.providerData: %@", authData.providerData);
                    NSDictionary *cachedUserProfile = [authData.providerData objectForKey:@"cachedUserProfile"];
                    NSString *thumImg = [cachedUserProfile objectForKey:@"headimgurl"];
                    NSString *nickname = [cachedUserProfile objectForKey:@"nickname"];
                    
                    SideMenuViewController *sv = (SideMenuViewController *)_sideMenuTabBarViewController.leftMenuViewController;
                    
                    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                        
                        NSURL *url = [NSURL URLWithString:thumImg];
                        NSData *data = [NSData dataWithContentsOfURL:url];
                        UIImage *img = [UIImage imageWithData:data];
                        dispatch_async(dispatch_get_main_queue(), ^{
                            
                            if (data != nil) {
                                sv.mThumImg.image = img;
                                
                            }else {
                                sv.mThumImg.image = [UIImage imageNamed:@"touxiang"];
                            }

                            sv.mThumImg.layer.cornerRadius = sv.mThumImg.frame.size.width/2;
                            sv.mThumImg.layer.borderColor = [[UIColor whiteColor] CGColor];
                            sv.mThumImg.layer.borderWidth = 2;
                            sv.mThumImg.layer.masksToBounds = YES;
                            
                        });
                        
                    });
                    
                    sv.mNickNameLab.text = nickname;
                    
                } withCancelBlock:^(NSError *error) {
                    
                    if (error) {
                        
                        [Utils hidenProgressHUdshowInView:self.window];
                        [Utils showStatusTextWithText:@"登录失败" showInView:self.window];
                        
                    }
                    
                }];
                
            }else {
                
                [Utils hidenProgressHUdshowInView:self.window];
                [Utils showStatusTextWithText:@"登录失败" showInView:self.window];

            }
        }];
    }
}
#pragma mark - WeiboSDKDelegate
- (void)didReceiveWeiboRequest:(WBBaseRequest *)request
{
    
}

- (void)didReceiveWeiboResponse:(WBBaseResponse *)response
{
    if ([response isKindOfClass:WBAuthorizeResponse.class])
    {
        WBAuthorizeResponse *wbResponse = (WBAuthorizeResponse *)response;
        if (wbResponse.accessToken == nil || wbResponse.userID == nil) {
            return;
        }

        [ApplicationDelegate.wilddog authWithOAuthProvider:@"weibo" parameters:@{@"access_token":wbResponse.accessToken,@"userID":wbResponse.userID} withCompletionBlock:^(NSError *error, WAuthData *authData) {
            
            [Utils hidenProgressHUdshowInView:self.window];
             
            if (authData) {
                
                [UserDefaults setObject:authData.uid forKey:UID];
                [UserDefaults setObject:authData.token forKey:TOKEN];
                [UserDefaults synchronize];
                
                NSLog(@"authData.uid : %@", authData.uid );
                
                Wilddog *manageWilddog = [ApplicationDelegate.wilddog childByAppendingPath:@"devicemanage"];
                Wilddog *uidWilddog = [manageWilddog childByAppendingPath:authData.uid];
                
                [uidWilddog observeSingleEventOfType:WEventTypeValue withBlock:^(WDataSnapshot *snapshot) {
                    
                    [Utils showStatusTextWithText:@"登录成功" showInView:self.window];
                    
                    NSInteger deviceCount = [snapshot.value count];
                    
                    NSDictionary *deviceDict = [[NSDictionary alloc] initWithDictionary:snapshot.value];
                    
                    NSMutableArray *deviceArr = [[NSMutableArray alloc] init];
                    [deviceDict enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
                        
                        
                        deviceItem *item = [[deviceItem alloc] init];
                        item.deviceID = key;
                        item.deviceBlue = [[obj objectForKey:@"led"] objectForKey:@"blue"];
                        item.deviceRed = [[obj objectForKey:@"led"] objectForKey:@"red"];
                        item.deviceGreen = [[obj objectForKey:@"led"] objectForKey:@"green"];
                        item.deviceSW = [[obj objectForKey:@"led"] objectForKey:@"sw"];
                        item.deviceNickName = [UserDefaults objectForKey:[NSString stringWithFormat:@"%ld%@",[item.deviceID hash], item.deviceID]];
                        
                        [deviceArr addObject:item];
                        
                    }];
                    
                    /*
                     没有绑定任何设备则为进入设备搜索页
                     有绑定设备进入设备管理页
                     */
                    if (0 == deviceCount) {
                        
                        WDDeviceManagerViewController *deviceManagerVC = [[WDDeviceManagerViewController alloc] initWithNibName:@"WDDeviceManagerViewController" bundle:nil];
                        [_navContent pushViewController:deviceManagerVC animated:NO];
                        
                        [deviceManagerVC pushSearchViewController];
                        
                    }else {
                        
                        WDDeviceManagerViewController *deviceManagerVC = [[WDDeviceManagerViewController alloc] initWithNibName:@"WDDeviceManagerViewController" bundle:nil];
                        [deviceManagerVC refreshDeviceListWith:deviceArr];
                        [_navContent.topViewController.navigationController pushViewController:deviceManagerVC animated:YES];
                        
                    }
                    
                    NSLog(@"authData.providerData: %@", authData.providerData);
                    NSString *nickname = [authData.providerData objectForKey:@"username"];
                    NSDictionary *cachedUserProfile = [authData.providerData objectForKey:@"cachedUserProfile"];
                    NSString *thumImg = [cachedUserProfile objectForKey:@"avatar_hd"];
                    
                    
                    SideMenuViewController *sv = (SideMenuViewController *)_sideMenuTabBarViewController.leftMenuViewController;
                    
                    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                        
                        NSURL *url = [NSURL URLWithString:thumImg];
                        NSData *data = [NSData dataWithContentsOfURL:url];
                        UIImage *img = [UIImage imageWithData:data];
                        dispatch_async(dispatch_get_main_queue(), ^{
                            
                            if (data != nil) {
                                sv.mThumImg.image = img;
                                
                            }else {
                                sv.mThumImg.image = [UIImage imageNamed:@"touxiang"];
                            }
                            
                            sv.mThumImg.layer.cornerRadius = sv.mThumImg.frame.size.width/2;
                            sv.mThumImg.layer.borderColor = [[UIColor whiteColor] CGColor];
                            sv.mThumImg.layer.borderWidth = 2;
                            sv.mThumImg.layer.masksToBounds = YES;
                            
                        });
                        
                    });
                    
                    sv.mNickNameLab.text = nickname;
                    
                } withCancelBlock:^(NSError *error) {
                    
                    if (error) {
                        
                        [Utils hidenProgressHUdshowInView:self.window];
                        [Utils showStatusTextWithText:@"登录失败" showInView:self.window];
                        
                    }
                    
                }];
                
            }else {
                
                [Utils hidenProgressHUdshowInView:self.window];
                [Utils showStatusTextWithText:@"登录失败" showInView:self.window];
                
            }
        }];
    }
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}



@end

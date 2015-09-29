//
//  WDLoginViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDLoginViewController.h"

#import <TencentOpenAPI/TencentOAuth.h>
#import "WXApi.h"
#import "WeiboSDK.h"
#import "AppDelegate.h"
#import "Utils.h"
#import "UIColor+Util.h"
#import "MBProgressHUD.h"
#import "WDRegisterViewController.h"
#import "WDForgetPwViewController.h"
#import "WDDeviceSearchViewController.h"
#import "WDDeviceManagerViewController.h"
#import "SideMenuViewController.h"
#import "RESideMenu.h"
#import "deviceItem.h"

@interface WDLoginViewController ()<TencentSessionDelegate,RegisterProtol,UIAlertViewDelegate,UITextFieldDelegate>
{
    TencentOAuth *_tencentOAuth;
    NSArray *_permissions;
}
@property (weak, nonatomic) IBOutlet UIView *mContentView;
@property (weak, nonatomic) IBOutlet UIImageView *mLogoView;
@property (weak, nonatomic) IBOutlet UIView *mUserNameView;
@property (weak, nonatomic) IBOutlet UIView *mPasswordView;
@property (weak, nonatomic) IBOutlet UIImageView *mUserImg;
@property (weak, nonatomic) IBOutlet UIImageView *mPasswordImg;

@property (weak, nonatomic) IBOutlet UITextField *mUserNameTF;
@property (weak, nonatomic) IBOutlet UITextField *mPasswordTF;

@property (weak, nonatomic) IBOutlet UIButton *mSocialLoginBtn;

@property (weak, nonatomic) IBOutlet UIView *mSocialLoginView;
@end


@implementation WDLoginViewController

#pragma mark - Keyboard handling

- (void)viewWillAppear:(BOOL)animated
{
    self.navigationController.navigationBarHidden = YES;
    self.navigationController.navigationBar.barTintColor = [UIColor colorWithHex:0x30b8ff];
    
    
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(keyboardWillShow:)
     name:UIKeyboardWillShowNotification object:nil];
    
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(keyboardWillHide:)
     name:UIKeyboardWillHideNotification object:nil];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter]
     removeObserver:self name:UIKeyboardWillShowNotification object:nil];
    
    [[NSNotificationCenter defaultCenter]
     removeObserver:self name:UIKeyboardWillHideNotification object:nil];
}

- (void)keyboardWillShow:(NSNotification*)notification
{
    [self moveView:[notification userInfo] up:YES];
}

- (void)keyboardWillHide:(NSNotification*)notification
{
    [self moveView:[notification userInfo] up:NO];
}

- (void)moveView:(NSDictionary*)userInfo up:(BOOL)up
{
    CGRect keyboardEndFrame;
    [[userInfo objectForKey:UIKeyboardFrameEndUserInfoKey]
     getValue:&keyboardEndFrame];
    
    UIViewAnimationCurve animationCurve;
    [[userInfo objectForKey:UIKeyboardAnimationCurveUserInfoKey]
     getValue:&animationCurve];
    
    NSTimeInterval animationDuration;
    [[userInfo objectForKey:UIKeyboardAnimationDurationUserInfoKey]
     getValue:&animationDuration];
    
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationBeginsFromCurrentState:YES];
    [UIView setAnimationDuration:animationDuration];
    [UIView setAnimationCurve:animationCurve];
    
    CGRect keyboardFrame = [self.view convertRect:keyboardEndFrame toView:nil];
    
    if (SCREEN_HEIGHT == 480) {
        
        int  y = keyboardFrame.origin.y - self.mPasswordView.frame.size.height  - 80;
        
        y = up ? -y : 0;
        
        [UIView animateKeyframesWithDuration:0.1 delay:0 options:UIViewKeyframeAnimationOptionLayoutSubviews animations:^{
            
            self.mLogoView.hidden = up;
            
        } completion:^(BOOL finished) {
            
        }];
        
        self.mContentView.frame = CGRectMake(0, y, self.mContentView.frame.size.width, self.mContentView.frame.size.height);
        
    }
    
    
    
    [UIView commitAnimations];
}

#pragma -mark RegisterProtol
-(void)RegisterSuccessWithAccount:(NSString *)accout andPassword:(NSString *)Password
{
    
    self.mUserNameTF.text = accout;
    self.mPasswordTF.text = Password;
    
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.

    self.mUserNameView.layer.cornerRadius = 6;
    self.mUserNameView.layer.borderWidth = 1;
    self.mUserNameView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
    
    self.mPasswordView.layer.cornerRadius = 6;
    self.mPasswordView.layer.borderWidth = 1;
    self.mPasswordView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
    
    [self.mSocialLoginBtn setTitleColor:[UIColor colorWithHex:0xb1ddf8] forState:UIControlStateNormal];
    
    self.mUserNameTF.text = [Utils readLastLoginAccount];
    
}

-(BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
    if (textField == self.mUserNameTF) {
        
        self.mUserNameView.layer.borderColor = [UIColor colorWithHex:0x16bef9].CGColor;
        self.mUserImg.image  = [UIImage imageNamed:@"yonghumingblue"];
        
    }else{
        
        self.mPasswordView.layer.borderColor = [UIColor colorWithHex:0x16bef9].CGColor;
        self.mPasswordImg.image  = [UIImage imageNamed:@"passwordiconblue"];
        
    }
    
    return YES;
}

-(BOOL)textFieldShouldEndEditing:(UITextField *)textField
{
    
    if (textField == self.mUserNameTF) {
        
        self.mUserNameView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mUserImg.image  = [UIImage imageNamed:@"yonghuminghui"];

        
    }else{
        
        self.mPasswordView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mPasswordImg.image  = [UIImage imageNamed:@"passwordicon"];

    }
    
    return YES;
}


- (IBAction)clickSocialLoginBtn:(UIButton *)sender {
    
    
    if (self.mSocialLoginView.hidden) {
        
        [UIView animateWithDuration:0.1 animations:^{
            
            self.mSocialLoginBtn.frame = CGRectMake(self.mSocialLoginBtn.frame.origin.x, self.mSocialLoginBtn.frame.origin.y-22, self.mSocialLoginBtn.frame.size.width, self.mSocialLoginBtn.frame.size.height);
            
        } completion:^(BOOL finished) {
            
            
            self.mSocialLoginView.hidden = NO;
           
            
        }];
 
    }else {
    
        [UIView animateWithDuration:0.1 animations:^{
            
            self.mSocialLoginBtn.frame = CGRectMake(self.mSocialLoginBtn.frame.origin.x, self.mSocialLoginBtn.frame.origin.y+22, self.mSocialLoginBtn.frame.size.width, self.mSocialLoginBtn.frame.size.height);
            
        } completion:^(BOOL finished) {
            
            
            self.mSocialLoginView.hidden = YES;
    
        }];

    }
    
}

#pragma -mark 点击空白隐藏键盘
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (self.mPasswordTF.editing) {
        
        self.mPasswordView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mPasswordImg.image  = [UIImage imageNamed:@"passwordicon"];
        
    } else if(self.mUserNameTF.editing){
        
        self.mUserNameView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mUserImg.image  = [UIImage imageNamed:@"yonghuminghui"];
        
    }
    

    
    [self.mPasswordTF resignFirstResponder];
    [self.mUserNameTF resignFirstResponder];
    
}

- (IBAction)clickForgetPwBtn:(UIButton *)sender {
    
    WDForgetPwViewController *forgetPsViewController = [[WDForgetPwViewController alloc] initWithNibName:@"WDForgetPwViewController" bundle:nil];
    [self.navigationController pushViewController:forgetPsViewController animated:YES];
}



- (IBAction)clickLoginBtn:(UIButton *)sender {

    if ([Utils isEmptyWithStr:self.mUserNameTF.text]) {
        
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:@"请输入账号邮箱!" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        return ;
    }
    
    if ([Utils isEmptyWithStr:self.mPasswordTF.text]) {
        
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:@"请输入账号密码!" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        return ;
    }
    
    BOOL email =  [Utils isValidateEmail:self.mUserNameTF.text];
    if (!email) {
        
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:@"邮箱格式错误!" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        return;
    }
    
   [Utils showProgressHUdWithText:@"正在登录" showInView:self.view];
    
   [Utils saveLastLoginAccount:self.mUserNameTF.text];

   [ApplicationDelegate.wilddog authUser:self.mUserNameTF.text password:self.mPasswordTF.text withCompletionBlock:^(NSError *error, WAuthData *authData) {
 
       [Utils hidenProgressHUdshowInView:self.view];
       
       if (authData) {
           
           [UserDefaults setObject:authData.uid forKey:UID];
           [UserDefaults setObject:authData.token forKey:TOKEN];
           [UserDefaults synchronize];
           
           NSLog(@"");
           Wilddog *manageWilddog = [ApplicationDelegate.wilddog childByAppendingPath:@"devicemanage"];
           Wilddog *uidWilddog = [manageWilddog childByAppendingPath:[UserDefaults objectForKey:UID]];
           
           [uidWilddog observeSingleEventOfType:WEventTypeValue withBlock:^(WDataSnapshot *snapshot) {
               
               [Utils hidenProgressHUdshowInView:self.view];
               [Utils showStatusTextWithText:@"登录成功" showInView:self.view];
               
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
                   [self.navigationController pushViewController:deviceManagerVC animated:NO];
                   
                   [deviceManagerVC pushSearchViewController];
                   
               }else {
                   
                   
                   WDDeviceManagerViewController *deviceManagerVC = [[WDDeviceManagerViewController alloc] initWithNibName:@"WDDeviceManagerViewController" bundle:nil];
                   [deviceManagerVC refreshDeviceListWith:deviceArr];
                   [self.navigationController pushViewController:deviceManagerVC animated:YES];
                   
               }
               
               
           } withCancelBlock:^(NSError *error) {
               
               
               [Utils hidenProgressHUdshowInView:self.view];
               if (error) {
                   [Utils showStatusTextWithText:@"登录失败" showInView:self.view];
               }
               
               
           }];
           
           
           SideMenuViewController *sv = (SideMenuViewController *)self.sideMenuViewController.leftMenuViewController;
           sv.mThumImg.image = [UIImage imageNamed:@"touxiang"];
           sv.mThumImg.layer.cornerRadius = sv.mThumImg.frame.size.width/2;
           sv.mThumImg.layer.borderColor = [[UIColor whiteColor] CGColor];
           sv.mThumImg.layer.borderWidth = 2;
           sv.mThumImg.layer.masksToBounds = YES;
           sv.mNickNameLab.text = self.mUserNameTF.text;
           
       }else {
           
           
           [Utils hidenProgressHUdshowInView:self.view];
           [Utils showStatusTextWithText:@"登录失败" showInView:self.view];
           
           
       }
       
   }];

}

- (IBAction)clickRegisterBtn:(UIButton *)sender {
    
    WDRegisterViewController *registerViewController = [[WDRegisterViewController alloc] initWithNibName:@"WDRegisterViewController" bundle:nil];
    [self.navigationController pushViewController:registerViewController animated:YES];
    
}



- (IBAction)clickQQLoginBtn:(UIButton *)sender {
    
    [Utils showProgressHUdWithText:@"正在登录" showInView:self.view];
    
    _tencentOAuth = [[TencentOAuth alloc]initWithAppId:QQ_KEY andDelegate:self];
    
    _permissions = [NSArray arrayWithObjects:@"get_user_info", @"get_simple_userinfo", @"add_t", nil];
    
    [_tencentOAuth authorize:_permissions inSafari:NO];
    
}



//登录成功：
- (void)tencentDidLogin
{
    [ApplicationDelegate.wilddog authWithOAuthProvider:@"qq" parameters:@{@"access_token":_tencentOAuth.accessToken,@"openId":_tencentOAuth.openId} withCompletionBlock:^(NSError *error, WAuthData *authData) {
        
        [Utils hidenProgressHUdshowInView:self.view];
        
        if (authData) {
            
            [UserDefaults setObject:authData.uid forKey:UID];
            [UserDefaults setObject:authData.token forKey:TOKEN];
            [UserDefaults synchronize];
            
            Wilddog *manageWilddog = [ApplicationDelegate.wilddog childByAppendingPath:@"devicemanage"];
            Wilddog *uidWilddog = [manageWilddog childByAppendingPath:authData.uid];
            
            [uidWilddog observeSingleEventOfType:WEventTypeValue withBlock:^(WDataSnapshot *snapshot) {
                
                [Utils showStatusTextWithText:@"登录成功" showInView:self.view];
                
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
                    [self.navigationController pushViewController:deviceManagerVC animated:NO];
                    
                    [deviceManagerVC pushSearchViewController];

                    
                }else {
                    
                    
                    WDDeviceManagerViewController *deviceManagerVC = [[WDDeviceManagerViewController alloc] initWithNibName:@"WDDeviceManagerViewController" bundle:nil];
                    [deviceManagerVC refreshDeviceListWith:deviceArr];
                    [self.navigationController pushViewController:deviceManagerVC animated:YES];
                    
                }
                
                
            } withCancelBlock:^(NSError *error) {
                
                [Utils hidenProgressHUdshowInView:self.view];
                
                [Utils showStatusTextWithText:@"登录失败" showInView:self.view];
                
            }];
            

            NSLog(@"authData.providerData: %@", authData.providerData);
            NSDictionary *cachedUserProfile = [authData.providerData objectForKey:@"cachedUserProfile"];
            NSString *thumImg = [cachedUserProfile objectForKey:@"figureurl_1"];
            NSString *nickname = [cachedUserProfile objectForKey:@"nickname"];

            
            SideMenuViewController *sv = (SideMenuViewController *)self.sideMenuViewController.leftMenuViewController;
    
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

            
            
        }else {
            
            [Utils hidenProgressHUdshowInView:self.view];
            [Utils showStatusTextWithText:@"登录失败" showInView:self.view];
        }
    }];
}

//非网络错误导致登录失败：
-(void)tencentDidNotLogin:(BOOL)cancelled
{
    [Utils hidenProgressHUdshowInView:self.view];
}
//网络错误导致登录失败：
-(void)tencentDidNotNetWork
{
    [Utils hidenProgressHUdshowInView:self.view];
}

- (IBAction)clickWXLoginBtn:(UIButton *)sender {
    
    [Utils showProgressHUdWithText:@"正在登录" showInView:ApplicationDelegate.window];
    
    SendAuthReq* req =[[SendAuthReq alloc] init];
    req.scope = @"snsapi_userinfo" ;
    req.state = @"123";
    //第三方向微信终端发送一个SendAuthReq消息结构
    [WXApi sendReq:req];
    
}

- (IBAction)clickXLWBLoginBtn:(UIButton *)sender {
    
    [Utils showProgressHUdWithText:@"正在登录" showInView:ApplicationDelegate.window];
    
    WBAuthorizeRequest *request = [WBAuthorizeRequest request];
    request.redirectURI = @"https://api.weibo.com/oauth2/default.html";
    request.scope = @"email,direct_messages_write";
    request.userInfo = @{@"SSO_From": @"WDLoginViewController",
                         @"Other_Info_1": [NSNumber numberWithInt:123], @"Other_Info_2": @[@"obj1", @"obj2"],
                         @"Other_Info_3": @{@"key1": @"obj1", @"key2": @"obj2"}};
    [WeiboSDK sendRequest:request];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 0) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[WXApi getWXAppInstallUrl]]];
    }
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end

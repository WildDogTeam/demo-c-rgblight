//
//  WDDeviceSearchViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDDeviceSearchViewController.h"

#import "WDSeekViewController.h"
#import "UIColor+Util.h"
#import "Utils.h"

#import <stdio.h>
#import "Reachability.h"
#import <SystemConfiguration/CaptiveNetwork.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

@interface WDDeviceSearchViewController ()<UITextFieldDelegate>
{
    WDSeekViewController *seekViewController;
    
    NSString *wifiName;
}

@property (weak, nonatomic) IBOutlet UIImageView *mLogoView;

@property (weak, nonatomic) IBOutlet UIView *mWiFiView;
@property (weak, nonatomic) IBOutlet UIView *mWiFiPsView;
@property (weak, nonatomic) IBOutlet UITextField *mWiFiTF;
@property (weak, nonatomic) IBOutlet UITextField *mWiFiPsTF;
@property (weak, nonatomic) IBOutlet UIImageView *mWiFiImg;
@property (weak, nonatomic) IBOutlet UIImageView *mWiFiPsImg;

@property (weak, nonatomic) IBOutlet UIButton *mDeviceBtn;
@property (weak, nonatomic) IBOutlet UIView *mSeekView;

@end


@implementation WDDeviceSearchViewController

-(id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        
       self.title = @"智能彩灯";
        
    }
    return self;
}

#pragma mark - Keyboard handling

- (void)viewWillAppear:(BOOL)animated
{
    self.navigationController.navigationBarHidden = NO;

    [[NSNotificationCenter defaultCenter]
     addObserver:self selector:@selector(keyboardWillShow:)
     name:UIKeyboardWillShowNotification object:nil];
    
    [[NSNotificationCenter defaultCenter]
     addObserver:self selector:@selector(keyboardWillHide:)
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
        
        int  y = keyboardFrame.origin.y - self.mWiFiPsView.frame.size.height  - 60;
        
        y = up ? -y : 0;
        
        [UIView animateKeyframesWithDuration:0.1 delay:0 options:UIViewKeyframeAnimationOptionLayoutSubviews animations:^{
            
            self.mLogoView.hidden = !self.mLogoView.hidden;
            
        } completion:^(BOOL finished) {
            
        }];
        
        self.view.frame = CGRectMake(0, y, self.view.frame.size.width, self.view.frame.size.height);
    }
    
    [UIView commitAnimations];
}


-(void)setContentView
{
    
    self.navigationItem.leftBarButtonItem  = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"Backicon"] style:UIBarButtonItemStylePlain target:self action:@selector(clickNavBack)];
    

    self.mWiFiView.layer.cornerRadius = 6;
    self.mWiFiView.layer.borderWidth = 1;
    self.mWiFiView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
    
    self.mWiFiPsView.layer.cornerRadius = 6;
    self.mWiFiPsView.layer.borderWidth = 1;
    self.mWiFiPsView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
    
}

-(void)seekSSID
{
    if ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] != NotReachable) {
        NSLog(@"Wifi connect");
        
        NSArray *ifs = (__bridge id)CNCopySupportedInterfaces();
        id info = nil;
        for (NSString *ifnam in ifs) {
            info = (__bridge id)CNCopyCurrentNetworkInfo((__bridge CFStringRef)ifnam);
            if(info && [info count]){
                NSDictionary *dic = (NSDictionary*)info; //取得網卡的資訊
                wifiName = [dic objectForKey:@"SSID"];   //取得ssid
                break;
            }
        }
        self.mWiFiTF.text = wifiName;
        
    }else{
        
        NSLog(@"No Wifi");
        UIAlertView *alert = [[UIAlertView alloc]
                              initWithTitle:@"未连接WIFI"
                              message:nil
                              delegate:nil
                              cancelButtonTitle:@"确定"
                              otherButtonTitles:nil, nil];
        [alert show] ;
        
    }
}


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [self setContentView];
    
    [self seekSSID];
    
}
   
-(BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
    if (textField == self.mWiFiTF) {
        
        self.mWiFiView.layer.borderColor = [UIColor colorWithHex:0x16bef9].CGColor;
        self.mWiFiImg.image  = [UIImage imageNamed:@"WiFiblue"];
        
    }else{
        
        self.mWiFiPsView.layer.borderColor = [UIColor colorWithHex:0x16bef9].CGColor;
        self.mWiFiPsImg.image  = [UIImage imageNamed:@"passwordiconblue"];
        
    }
    
    return YES;
}

-(BOOL)textFieldShouldEndEditing:(UITextField *)textField
{
    
    if (textField == self.mWiFiTF) {
        
        self.mWiFiView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mWiFiImg.image  = [UIImage imageNamed:@"WiFihui"];
        
        
    }else{
        
        self.mWiFiPsView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mWiFiPsImg.image  = [UIImage imageNamed:@"passwordicon"];
        
        
    }
    
    return YES;
    
    
}


- (IBAction)clickDeviceBtn:(UIButton *)sender {

    [self.mWiFiPsTF resignFirstResponder];
    [self.mWiFiTF resignFirstResponder];
    
    
    if ([Utils isEmptyWithStr:self.mWiFiTF.text]) {
        
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:@"请输入WIFI名称!" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        return ;
    }
    
    if ([Utils isEmptyWithStr:self.mWiFiPsTF.text]) {
        
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:@"请输入WIFI密码!" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        return ;
    }
    

    seekViewController = [[WDSeekViewController alloc] initWithNibName:@"WDSeekViewController" bundle:nil];
    seekViewController.mWiFiName = self.mWiFiTF.text;
    seekViewController.mWiFiPs = self.mWiFiPsTF.text;
    
    [self.navigationController pushViewController:seekViewController animated:YES];

}
    



-(void)clickNavBack
{
    [self.navigationController popViewControllerAnimated:YES];
}

#pragma -mark 点击空白隐藏键盘
-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self.mWiFiPsTF resignFirstResponder];
    [self.mWiFiTF resignFirstResponder];
    
    if (self.mWiFiPsTF.editing) {
        
        self.mWiFiPsView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mWiFiPsImg.image  = [UIImage imageNamed:@"passwordicon"];
        
    }else {
        
        self.mWiFiView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
        self.mWiFiImg.image  = [UIImage imageNamed:@"WiFihui"];
        
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

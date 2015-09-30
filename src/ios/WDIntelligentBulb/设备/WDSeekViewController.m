//
//  WDSeekViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/14.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDSeekViewController.h"

#import "AppDelegate.h"
#import <stdio.h>
#import "cooee.h"
#import "Reachability.h"
#import <SystemConfiguration/CaptiveNetwork.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#import "AsyncUdpSocket.h"

#import "Utils.h"
#import "NSTimer+Addition.h"

#import "WDDeviceManagerViewController.h"
#import "WDDeviceShowViewController.h"

#define   kUDP_PORT   53364


@interface WDSeekViewController ()<UIAlertViewDelegate>
{
    UIImageView *imageView11;
    UIImageView *imageView12;
    
    BOOL FastSend;
    
    const char *PWD;
    const char *SSID;
    const char *KEY;
    unsigned int ip;
    
    NSTimer *Send_cooee;
    NSTimer *sendBroadTimer;
    
    AsyncUdpSocket *asyncUdpSocket;
    
    NSString *deviceID;
}

@property (weak, nonatomic) IBOutlet UIView *mTopContentView;
@property (weak, nonatomic) IBOutlet UIImageView *mRippleImg;
@property (weak, nonatomic) IBOutlet UIImageView *mExceptionImg;

@property (weak, nonatomic) IBOutlet UIImageView *mRotateImg;
@property (weak, nonatomic) IBOutlet UIImageView *mLoopImg;


@property (weak, nonatomic) IBOutlet UILabel *mLabTitle;
@property (weak, nonatomic) IBOutlet UILabel *mLabContent;

@property (weak, nonatomic) IBOutlet UIButton *mBtnOn;
@property (weak, nonatomic) IBOutlet UIButton *mBtnOff;
@property (weak, nonatomic) IBOutlet UILabel *mOffLab;
@property (weak, nonatomic) IBOutlet UILabel *mOnLab;


@property(nonatomic, assign)BOOL mIsSeeking;
@property(nonatomic, assign)BOOL mSeekSuccess;

@property(nonatomic, strong)NSMutableDictionary *mDeviceData;

@property(nonatomic, strong)NSString *UDP_IP;

@property(nonatomic, assign)NSInteger mSendBroadCount;
@end


@implementation WDSeekViewController


-(id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if ( self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        
        self.mDeviceData = [[NSMutableDictionary alloc] init];
        self.mIsSeeking = YES;
        self.mSeekSuccess = NO;
        
    }
    return self;
}

-(void)viewWillAppear:(BOOL)animated
{
    
    self.navigationController.navigationBarHidden = NO;
    self.navigationItem.leftBarButtonItem  = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"Backicon"] style:UIBarButtonItemStylePlain target:self action:@selector(clickNavBack)];
    
    self.navigationItem.title = @"智能彩灯";
    
    //界面初始化设置
    self.mBtnOn.hidden = YES;
    self.mOffLab.text = @"取消";
    
    [self rotateLeiDa];
    [self loopRipple];
    
}

-(void)viewDidDisappear:(BOOL)animated
{
    //停止UDP
    [sendBroadTimer pauseTimer];
    
    
    //停止快连
    [self pauseFastLink];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(disappearSelector) name:UIApplicationWillResignActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appearSelector) name:UIApplicationWillEnterForegroundNotification object:nil];
    
    //开始快连
    [self startFastLink];
    //发送UDP广播
    [self sendBroadUDP];
}


-(void)appearSelector
{
    

    
}

-(void)disappearSelector{
    NSLog(@"disappearSelector");
    
    if(self.mIsSeeking){
        [self seekDeviceFail];
    }
}


#pragma -martk 获取设备列表

-(void)ListenDevice
{
    
    for (UIViewController *controller in self.navigationController.viewControllers) {
        
        if ([controller isKindOfClass:[WDDeviceManagerViewController class]]) {
            
            [self.navigationController popToViewController:controller animated:YES];
            
        }
        
    }
    
}

#pragma -mark 发送广播
-(void)sendBroadUDP
{
    self.mSendBroadCount = 0;//广播次数
    self.UDP_IP = @"255.255.255.255";//默认广播
    
    asyncUdpSocket = [[AsyncUdpSocket alloc] initWithDelegate:self];
    //绑定端口
    NSError *err = nil;
    [asyncUdpSocket bindToPort:kUDP_PORT error:&err];
    
    //发送广播设置
    [asyncUdpSocket enableBroadcast:YES error:&err];
    
    //启动接收线程
    [asyncUdpSocket receiveWithTimeout:-1 tag:0];
    
    //发送广播
    sendBroadTimer =  [NSTimer scheduledTimerWithTimeInterval:5.0 target:self selector:@selector(broadUDP) userInfo:nil repeats:YES];
    
}

//发送广播，通知其他客户端
-(void)broadUDP
{
    
    NSDictionary *dictParm;
    if (self.mSeekSuccess) {
        
        //绑定设备时
        dictParm =   @{@"cmd":@"set",@"subcmd":@{@"token":[UserDefaults objectForKey:TOKEN],@"userId":[UserDefaults objectForKey:UID]}};
        
    }else {
        
        //寻找设备时
        dictParm =  @{@"cmd":@"get",@"subcmd":@{@"devId":@"0"}};
        
    }
    
    //发送广播
    [asyncUdpSocket sendData:[Utils toJSONData:dictParm]
                      toHost:self.UDP_IP
                        port:kUDP_PORT
                 withTimeout:-1
                         tag:0];
    
    
    if (self.mIsSeeking && self.mSendBroadCount > 12) {
       //*********寻找设备失败
        
        //停止快连
        [self pauseFastLink];
        //停止广播
        [sendBroadTimer pauseTimer];
        
        if (!self.mSeekSuccess) {
            [self seekDeviceFail];
        }
        
    }else if (!self.mIsSeeking && self.mSendBroadCount > 12){
    //**********绑定失败
        
        //停止广播
        [sendBroadTimer pauseTimer];
        
        if (self.mSeekSuccess) {
            
            [Utils hidenProgressHUdshowInView:self.view];
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"绑定失败" message:nil delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
            [alertView show];
            
        }
        
    }
    
    
    NSLog(@"001wait time :%ld",(long)self.mSendBroadCount);
    
    self.mSendBroadCount = self.mSendBroadCount + 1;
    
    NSLog(@"002wait time :%ld",(long)self.mSendBroadCount);

}

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == 1111) {
        
        //监听设备节点的变化
        [self performSelector:@selector(ListenDevice) withObject:nil afterDelay:1.0];
    }
}

//已接收到消息
- (BOOL)onUdpSocket:(AsyncUdpSocket *)sock didReceiveData:(NSData *)data withTag:(long)tag fromHost:(NSString *)host port:(UInt16)port{
    
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
    
    NSLog(@"dict: %@", dict);
    
    NSString *cmd = [dict objectForKey:@"cmd"];
    if ([cmd isEqualToString:@"ack"]) {
        
        id token = [[dict objectForKey:@"subcmd"] objectForKey:@"token"];
        if (token && [token isEqualToString:@"success"] && self.mSeekSuccess) {
            
            
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"绑定成功" message:nil delegate:self cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
            alertView.tag = 1111;
            
            [alertView show];
            
            [sendBroadTimer pauseTimer];
            
        }else if(!self.mSeekSuccess){
            
            [self.mDeviceData removeAllObjects];
            [self.mDeviceData addEntriesFromDictionary:dict];
            
            [self seekDeviceSuccess];
            
            self.UDP_IP = host;
            
            
            deviceID = [[dict objectForKey:@"subcmd"] objectForKey:@"devId"];
            
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"寻找设备成功" message:deviceID delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
            [alertView show];

        }
    }
    
    //启动接收线程
    [asyncUdpSocket receiveWithTimeout:-1 tag:0];
    
    return YES;

}

//没有接受到消息
-(void)onUdpSocket:(AsyncUdpSocket *)sock didNotReceiveDataWithTag:(long)tag dueToError:(NSError *)error{
    
}


//没有发送出消息
-(void)onUdpSocket:(AsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error{
    
    
}


//已发送出消息
-(void)onUdpSocket:(AsyncUdpSocket *)sock didSendDataWithTag:(long)tag{
    
}

//断开连接
-(void)onUdpSocketDidClose:(AsyncUdpSocket *)sock{
    
    
}


#pragma -mark 寻找设备成功

-(void)seekDeviceSuccess
{
    //状态设置
    self.mIsSeeking = NO;
    self.mSeekSuccess = YES;
    
    //停止UDP
    [sendBroadTimer pauseTimer];
    //停止快连
    [self pauseFastLink];
    
    //设置灯视图
    [self rotateLeiDa];
    [self loopRipple];
    
    
    
    self.mBtnOn.hidden = NO;
    [self.mBtnOff setImage:[UIImage imageNamed:@"fanhui"] forState:UIControlStateNormal];
    self.mOnLab.text = @"绑定";
    self.mOffLab.text = @"返回";
    
    self.mLabTitle.text = @"发现设备";
    self.mLabContent.text = [NSString stringWithFormat:@"设备名称: %@", [[self.mDeviceData objectForKey:@"subcmd"] objectForKey:@"devId"] ];
    
}

#pragma -mark 寻找设备失败

-(void)seekDeviceFail
{
    //状态设置
    self.mIsSeeking = NO;
    self.mSeekSuccess = NO;
    
    //停止UDP
    [sendBroadTimer pauseTimer];
    //停止快连
    [self pauseFastLink];
    
    //设置灯视图
    [self rotateLeiDa];
    [self loopRipple];
    
    
    
    self.mBtnOn.hidden = NO;
    self.mOnLab.text = @"重新寻找";
    [self.mBtnOff setImage:[UIImage imageNamed:@"fanhui"] forState:UIControlStateNormal];
    self.mOffLab.text = @"返回";
    self.mLabTitle.text = @"寻找失败";
    self.mLabContent.text = @"未找到可匹配的设备,请重新寻找";
    
}


#pragma -mark 开始快连
-(void)startFastLink
{
    FastSend = YES;
    [self sendFastLink];
}

#pragma -mark 停止快连
-(void)pauseFastLink
{
    FastSend = NO;
}


#pragma -mark 发送快连
-(void)sendFastLink
{
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        while (FastSend) {
            
            //  NSLog(@"StartCooee");
            
            PWD = [self.mWiFiPs UTF8String];
            SSID = [self.mWiFiName UTF8String];
            
            KEY = [@"" UTF8String];
            struct in_addr addr;
            inet_aton([[self getIPAddress] UTF8String], &addr);
            ip = CFSwapInt32BigToHost(ntohl(addr.s_addr));
            
            NSLog(@"\n\nSSID = %s" , SSID);
            NSLog(@"strlen(SSID) = %lu" , strlen(SSID));
            NSLog(@"PWD = %s" , PWD);
            NSLog(@"strlen(PWD) = %lu\n\n" , strlen(PWD));
 
            send_cooee(SSID, (int)strlen(SSID), PWD, (int)strlen(PWD), KEY, 0, ip);
        }
        
    });
    
}

- (NSString *)getIPAddress
{
    NSString *address = @"error";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0)
    {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL)
        {
            if(temp_addr->ifa_addr->sa_family == AF_INET)
            {
                // Check if interface is en0 which is the wifi connection on the iPhone
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"])
                {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    
    // Free memory
    freeifaddrs(interfaces);
    
    return address;
}



-(void)loopRipple
{
    
    if (imageView11) {
        [imageView11 removeFromSuperview];
        [imageView12 removeFromSuperview];
        
        imageView11 = nil;
        imageView11 = nil;
    }
    
    NSString *imgName = self.mIsSeeking? @"guangquanlan":@"guangquanhuang";
    
    
    imageView11 = [[UIImageView alloc] initWithImage:[UIImage imageNamed: imgName]];
    imageView12 = [[UIImageView alloc] initWithImage:[UIImage imageNamed: imgName]];
    
    imageView11.center = self.mLoopImg.center;
    imageView12.center = self.mLoopImg.center;
    
    
    if (self.mSeekSuccess) {
        
        imageView11.image = [UIImage imageNamed:@"guangquanlan"];
        [self.view insertSubview:imageView11 belowSubview:self.mRotateImg];
        imageView11.transform = CGAffineTransformMakeScale(1.2,1.2);
        
        return;
    }
    
    [self.view insertSubview:imageView11 belowSubview:self.mRotateImg];
    [self.view insertSubview:imageView12 belowSubview:self.mRotateImg];
    
    imageView11.transform = CGAffineTransformMakeScale(1.2,1.2);
    imageView12.transform = CGAffineTransformMakeScale(1.2,1.2);
    [UIView animateWithDuration:2 delay:0 options:UIViewAnimationOptionRepeat|UIViewAnimationOptionCurveEaseInOut animations:^{
        
        imageView11.transform = CGAffineTransformMakeScale(2,2);
        imageView11.alpha = .0;
        
        imageView12.transform = CGAffineTransformMakeScale(1.5,1.5);
        imageView12.alpha = .0;
        
    }completion:^(BOOL finished) {
        
    }];
    
}

-(void)rotateLeiDa
{

    NSString *imgName = self.mIsSeeking? @"leida":self.mSeekSuccess? @"logoblue":@"xunzhaoshibai";
   
    if (self.mIsSeeking) {
        self.mRotateImg.image = [UIImage imageNamed:imgName];
        self.mRotateImg.hidden = NO;
        self.mExceptionImg.hidden = YES;
    }else {
        
        [self.mRotateImg.layer removeAnimationForKey:@"transform.rotation.z"];
        self.mExceptionImg.image = [UIImage imageNamed:imgName];
        self.mRotateImg.hidden = YES;
        self.mExceptionImg.hidden = NO;
    }
    
    
    CABasicAnimation* rotationAnimation;
    rotationAnimation = [CABasicAnimation animationWithKeyPath:@"transform.rotation.z"];
    rotationAnimation.toValue = [NSNumber numberWithFloat: M_PI * 2.0 ];
    [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
    rotationAnimation.duration = 2;
    rotationAnimation.repeatCount =  self.mIsSeeking? 1000000000000:0;//你可以设置到最大的整数值
    rotationAnimation.cumulative = NO;
    rotationAnimation.removedOnCompletion = NO;
    rotationAnimation.fillMode = kCAFillModeForwards;
    [self.mRotateImg.layer addAnimation:rotationAnimation forKey:@"Rotation"];

}

- (IBAction)clickOn:(id)sender {

    if ([self.mOnLab.text isEqualToString:@"绑定"]) {
        //点击绑定
        
        [Utils showProgressHUdWithText:@"绑定中..." showInView:self.view];
        
        self.mSendBroadCount = 0;
        [sendBroadTimer resumeTimer];
        
    }else if ([self.mOnLab.text isEqualToString:@"重新寻找"]){
        //点击重新寻找
        
        self.mIsSeeking = YES;
        self.mSeekSuccess = NO;
        
        
        [self startFastLink];
        
        self.mSendBroadCount = 0;
        [sendBroadTimer resumeTimer];
        
        [self rotateLeiDa];
        [self loopRipple];
        
        
        self.mBtnOn.hidden = YES;
        
        [self.mBtnOff setImage:[UIImage imageNamed:@"xunzhaoshebei"] forState:UIControlStateNormal];
        self.mOffLab.text = @"取消";
        self.mLabTitle.text = @"寻找设备";
        self.mLabContent.text = @"请耐心等待,正在努力寻找设备中...";
    
    }
}

- (IBAction)clickCancel:(id)sender {
    
    if ([self.mOffLab.text isEqualToString:@"取消"]) {
        
        
        //关闭快连
        [self pauseFastLink];
        //关闭广播包
        [sendBroadTimer pauseTimer];
        
        
        self.mIsSeeking = NO;
        self.mSeekSuccess = NO;
        
        [self rotateLeiDa];
        [self loopRipple];
        
        
        self.mBtnOn.hidden = NO;
        [self.mBtnOff setImage:[UIImage imageNamed:@"fanhui"] forState:UIControlStateNormal];
        self.mOnLab.text = @"重新寻找";
        self.mOffLab.text = @"返回";
        self.mLabTitle.text = @"寻找失败";
        self.mLabContent.text = @"未找到可匹配的设备,请重新寻找";
        
        
    }else if([self.mOffLab.text isEqualToString:@"返回"]){
        
        [self.navigationController popViewControllerAnimated:YES];
        
    }
    
}

-(void)clickNavBack
{
    [sendBroadTimer invalidate];
    [self.navigationController popViewControllerAnimated:YES];
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

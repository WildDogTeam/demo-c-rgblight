//
//  WDDeviceShowViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/12.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDDeviceShowViewController.h"

#import "AppDelegate.h"


@interface WDDeviceShowViewController ()
{
    Wilddog *_ledWilddog;
    Wilddog *_deviceIDWilddog;
    
    UInt64 PastSecond;
}


@property (weak, nonatomic) IBOutlet UIButton *mLightSwitch;

@property (weak, nonatomic) IBOutlet UIView *mLightColorView;

@property (strong, nonatomic) IBOutlet UISlider *redSlider;
@property (strong, nonatomic) IBOutlet UISlider *greenSlider;
@property (strong, nonatomic) IBOutlet UISlider *blueSlider;
@property (weak, nonatomic) IBOutlet UILabel *mRedValueLab;
@property (weak, nonatomic) IBOutlet UILabel *mGreenValueLab;
@property (weak, nonatomic) IBOutlet UILabel *mBlueValueLab;

@end



@implementation WDDeviceShowViewController

-(id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        
        PastSecond = [[NSDate date] timeIntervalSince1970]*1000;
    }
    
    return self;
}

-(void)viewWillAppear:(BOOL)animated
{
    self.navigationController.navigationBarHidden = YES;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.mLightColorView.layer.cornerRadius = self.mLightColorView.frame.size.width/2;
    
    Wilddog *manageWilddog = [ApplicationDelegate.wilddog childByAppendingPath:@"devicemanage"];
    
    Wilddog *uidWilddog = [manageWilddog childByAppendingPath:[UserDefaults objectForKey:UID]];
    
    
    _deviceIDWilddog = [uidWilddog childByAppendingPath:self.mDeviceItem.deviceID];
    
    
    self.redSlider.continuous = NO;
    self.greenSlider.continuous = NO;
    self.blueSlider.continuous = NO;
    
    
    self.redSlider.value = [self.mDeviceItem.deviceRed floatValue]/255;
    self.greenSlider.value = [self.mDeviceItem.deviceGreen floatValue]/255;
    self.blueSlider.value = [self.mDeviceItem.deviceBlue floatValue]/255;
    
    
    self.mRedValueLab.text = [NSString stringWithFormat:@"%ld", [self.mDeviceItem.deviceRed integerValue]];
    self.mGreenValueLab.text = [NSString stringWithFormat:@"%ld", [self.mDeviceItem.deviceGreen integerValue]];
    self.mBlueValueLab.text = [NSString stringWithFormat:@"%ld", [self.mDeviceItem.deviceBlue integerValue]];
    
    
    [self.redSlider setThumbImage:[UIImage imageNamed:@"anniiu"] forState:UIControlStateNormal];
    [self.greenSlider setThumbImage:[UIImage imageNamed:@"anniiu"] forState:UIControlStateNormal];
    [self.blueSlider setThumbImage:[UIImage imageNamed:@"anniiu"] forState:UIControlStateNormal];
    
    
    UIColor *color = [UIColor colorWithRed:self.redSlider.value green:self.greenSlider.value blue:self.blueSlider.value alpha:1.0f];
    self.mLightColorView.backgroundColor = color;
    
    if ([self.mDeviceItem.deviceSW isEqualToString:@"0"]) {
        
        [self.mLightSwitch setImage:[UIImage imageNamed:@"off"] forState:UIControlStateNormal];
        
    }else {
        
        [self.mLightSwitch setImage:[UIImage imageNamed:@"on"] forState:UIControlStateNormal];
    }
   
    
    [_deviceIDWilddog observeEventType:WEventTypeValue withBlock:^(WDataSnapshot *snapshot) {
        
        if (snapshot.value) {
            
            NSDictionary *deviceDict = [[NSDictionary alloc] initWithDictionary:snapshot.value];
            NSDictionary *ledDict = [deviceDict objectForKey:@"led"];
            
            
            NSString *blueValue = [ledDict objectForKey:@"blue"];
            NSString *redValue = [ledDict objectForKey:@"red"];
            NSString *greenValue = [ledDict objectForKey:@"green"];
            NSString *swValue = [ledDict objectForKey:@"sw"];
            
            
            self.blueSlider.value = [blueValue floatValue]/255;
            self.mBlueValueLab.text = blueValue;
            
            self.redSlider.value = [redValue floatValue]/255;
            self.mRedValueLab.text = redValue;
            
            self.greenSlider.value = [greenValue floatValue]/255;
            self.mGreenValueLab.text = greenValue;
            
            
            
            NSLog(@"r: %@ g:%@ b:%@", self.mRedValueLab.text,self.mGreenValueLab.text,self.mBlueValueLab.text);
            
            if ([swValue isEqualToString:@"0"]) {
                [self.mLightSwitch setImage:[UIImage imageNamed:@"off"] forState:UIControlStateNormal];
            }else {
                [self.mLightSwitch setImage:[UIImage imageNamed:@"on"] forState:UIControlStateNormal];
            }
            
            
            UIColor *color = [UIColor colorWithRed:self.redSlider.value green:self.greenSlider.value blue:self.blueSlider.value alpha:1.0f];
            self.mLightColorView.backgroundColor = color;
            
        }
        
    }];
    

    
}


- (void)updateViewBackgroundColor {
    
    
    UInt64 msecond1 = [[NSDate date] timeIntervalSince1970]*1000;
    
    if (msecond1 - PastSecond < 200) {
        return ;
    }
    
    PastSecond = [[NSDate date] timeIntervalSince1970]*1000;
    
    
    UIColor *color = [UIColor colorWithRed:self.redSlider.value green:self.greenSlider.value blue:self.blueSlider.value alpha:1.0f];
    self.mLightColorView.backgroundColor = color;
    
    
    NSString *redValue = [NSString stringWithFormat:@"%.f", self.redSlider.value*255];
    NSString *greenValue = [NSString stringWithFormat:@"%.f", self.greenSlider.value*255];
    NSString *blueValue = [NSString stringWithFormat:@"%.f", self.blueSlider.value*255];

    NSDictionary *rgbsw = @{@"red":redValue,@"green":greenValue,@"blue":blueValue,@"sw":self.mDeviceItem.deviceSW};
    
    __weak typeof(deviceItem) *weakdeviceItem = _mDeviceItem;
    __weak typeof(UIButton) *weakmLightSwitch = _mLightSwitch;
    
    Wilddog *led = [_deviceIDWilddog childByAppendingPath:@"led"];
    [led updateChildValues:rgbsw withCompletionBlock:^(NSError *error, Wilddog *ref) {
        
        NSLog(@"开关变化02: %f\n\n\n",[[NSDate date] timeIntervalSince1970]);
        
        if (!error) {
            if ([weakdeviceItem.deviceSW isEqualToString:@"1"]) {
                [weakmLightSwitch setImage:[UIImage imageNamed:@"on"] forState:UIControlStateNormal];
                weakdeviceItem.deviceSW = @"1";

            }else {
                [weakmLightSwitch setImage:[UIImage imageNamed:@"off"] forState:UIControlStateNormal];
                weakdeviceItem.deviceSW = @"0";
            }
        }

    }];
    
}

- (IBAction)sliderValuesChanged:(id)sender {


    self.mRedValueLab.text = [NSString stringWithFormat:@"%.f", self.redSlider.value*255];
    self.mGreenValueLab.text = [NSString stringWithFormat:@"%.f", self.greenSlider.value*255];
    self.mBlueValueLab.text = [NSString stringWithFormat:@"%.f", self.blueSlider.value*255];
    
    [self updateViewBackgroundColor];
    
}


- (IBAction)clickLightOnBtn:(UIButton *)sender {
    
    if ([self.mDeviceItem.deviceSW isEqualToString:@"1"]) {
        
        self.mDeviceItem.deviceSW = @"0";
        
        
    }else {
        
        self.mDeviceItem.deviceSW = @"1";
        
    }
    
    [self updateViewBackgroundColor];
    
}


- (IBAction)clickNavBackBtn:(id)sender {
    
    [self.navigationController popViewControllerAnimated:YES];
    
}

-(void)alertShow:(NSString *)title
{
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:title delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
    [alertView show];
    
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

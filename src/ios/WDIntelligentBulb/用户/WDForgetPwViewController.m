//
//  WDForgetPwViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDForgetPwViewController.h"

#import "AppDelegate.h"
#import "Utils.h"
#import "UIColor+Util.h"

@interface WDForgetPwViewController ()<UITextFieldDelegate>

@property (weak, nonatomic) IBOutlet UITextField *mEmailTF;
@property (weak, nonatomic) IBOutlet UIImageView *mEmailImg;
@property (weak, nonatomic) IBOutlet UIView *mEmailView;

@property (weak, nonatomic) IBOutlet UIView *mContentView;
@property (weak, nonatomic) IBOutlet UIImageView *mLogoView;

@end

@implementation WDForgetPwViewController

#pragma mark - Keyboard handling

- (void)viewWillAppear:(BOOL)animated
{
    self.navigationController.navigationBarHidden = YES;
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
        
        int  y = keyboardFrame.origin.y - self.mEmailView.frame.size.height  - 80;
        
        y = up ? -y : 0;
        
        [UIView animateKeyframesWithDuration:0.1 delay:0 options:UIViewKeyframeAnimationOptionLayoutSubviews animations:^{
            
            self.mLogoView.hidden = !self.mLogoView.hidden;
            
        } completion:^(BOOL finished) {
            
        }];
        
        self.mContentView.frame = CGRectMake(0, y, self.mContentView.frame.size.width, self.mContentView.frame.size.height);
    }
    
    
    
    [UIView commitAnimations];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.mEmailView.layer.cornerRadius = 6;
    self.mEmailView.layer.borderWidth = 1;
    self.mEmailView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
    
  
    
}

-(BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
    self.mEmailView.layer.borderColor = [UIColor colorWithHex:0x16bef9].CGColor;
    self.mEmailImg.image  = [UIImage imageNamed:@"yonghumingblue"];
    
    return YES;
}

-(BOOL)textFieldShouldEndEditing:(UITextField *)textField
{
    
    self.mEmailView.layer.borderColor = [UIColor colorWithHex:0x9999990].CGColor;
    self.mEmailImg.image  = [UIImage imageNamed:@"yonghuminghui"];
    
    return YES;
    
    
}
- (IBAction)clickFindPasBtn:(UIButton *)sender {
    
    BOOL email =  [Utils isValidateEmail:self.mEmailTF.text];
    if (!email) {
        
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"邮箱错误" message:@"请输入正确邮箱号" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        return;
    }

    
    [Utils showProgressHUdWithText:@"正在发送..." showInView:self.view];
    [ApplicationDelegate.wilddog resetPasswordForUser:self.mEmailTF.text withCompletionBlock:^(NSError *error) {
        [Utils hidenProgressHUdshowInView:self.view];

        if (error) {
            [Utils showStatusTextWithText:@"发送失败" showInView:self.view];
        }else {
            [Utils showStatusTextWithText:@"发送成功" showInView:self.view];
        }
        
    }];
    
    
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    self.mEmailView.layer.borderColor = [UIColor colorWithHex:0x16bef9].CGColor;
    self.mEmailImg.image  = [UIImage imageNamed:@"yonghuminghui"];
    
    [self.mEmailTF resignFirstResponder];
}
- (IBAction)clickBackBtn:(id)sender {
    
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

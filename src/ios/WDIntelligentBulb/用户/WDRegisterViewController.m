//
//  WDRegisterViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDRegisterViewController.h"

#import "AppDelegate.h"
#import "Utils.h"
#import "UIColor+Util.h"

@interface WDRegisterViewController ()<UITextFieldDelegate>

@property (weak, nonatomic) IBOutlet UIView *mContentView;
@property (weak, nonatomic) IBOutlet UIImageView *mLogoView;
@property (weak, nonatomic) IBOutlet UIView *mUserNameView;
@property (weak, nonatomic) IBOutlet UIView *mPasswordView;
@property (weak, nonatomic) IBOutlet UIImageView *mUserImg;
@property (weak, nonatomic) IBOutlet UIImageView *mPasswordImg;

@property (weak, nonatomic) IBOutlet UITextField *mUserNameTF;
@property (weak, nonatomic) IBOutlet UITextField *mPasswordTF;


@end

@implementation WDRegisterViewController

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


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    self.mUserNameView.layer.cornerRadius = 6;
    self.mUserNameView.layer.borderWidth = 1;
    self.mUserNameView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
    
    self.mPasswordView.layer.cornerRadius = 6;
    self.mPasswordView.layer.borderWidth = 1;
    self.mPasswordView.layer.borderColor = [UIColor colorWithHex:0x999999].CGColor;
    
   
    
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


- (IBAction)clickRegisterBtn:(UIButton *)sender {
    
    
    BOOL email =  [Utils isValidateEmail:self.mUserNameTF.text];
    if (!email) {
        
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:@"请输入有效Email地址!" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        
        return;
    }
    
    if (self.mPasswordTF.text.length < 8) {
    
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil message:@"请输入至少8位字符作为密码!" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil, nil];
        [alertView show];
        
        return;
    }

    
    [Utils showProgressHUdWithText:@"正在注册" showInView:self.view];
    [ApplicationDelegate.wilddog createUser:self.mUserNameTF.text password:self.mPasswordTF.text withValueCompletionBlock:^(NSError *error, NSDictionary *result) {
        
        [Utils hidenProgressHUdshowInView:self.view];
        
        if (!error) {
            
            [Utils showStatusTextWithText:@"注册成功" showInView:self.view];
            
            [self.delegate RegisterSuccessWithAccount:self.mUserNameTF.text andPassword:self.mPasswordTF.text];
            
            [self performSelector:@selector(clickBack:) withObject:nil afterDelay:2];
            
         
            
        }else{

            [Utils showStatusTextWithText:@"注册失败" showInView:self.view];
 
        }
        
    }];

    
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

- (IBAction)clickBack:(id)sender {
    
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

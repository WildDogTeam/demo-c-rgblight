//
//  SideMenuViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/21.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "SideMenuViewController.h"

#import "RESideMenu.h"
#import "AppDelegate.h"

@interface SideMenuViewController ()

@end

@implementation SideMenuViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    
    
}

- (IBAction)clickLogoutBtn:(id)sender {
    
    [self.sideMenuViewController hideMenuViewController];
    
    //注销登录
    [ApplicationDelegate.wilddog unauth];
    
    UINavigationController *nav = (UINavigationController *)self.sideMenuViewController.contentViewController;
    [nav popToRootViewControllerAnimated:YES];
    
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

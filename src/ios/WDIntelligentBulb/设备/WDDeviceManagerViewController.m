//
//  WDDeviceManagerViewController.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/6.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "WDDeviceManagerViewController.h"
#import "WDDeviceSearchViewController.h"
#import "WDDeviceShowViewController.h"
#import "DAContextMenuCell.h"
#import "WDDeviceCell.h"
#import "UIColor+Util.h"
#import "deviceItem.h"
#import "RESideMenu.h"
#import "AlertBlock.h"
#import "AppDelegate.h"
#import "Utils.h"


@interface WDDeviceManagerViewController ()

@property(nonatomic, strong)NSMutableArray *dataArr;

@end


@implementation WDDeviceManagerViewController


-(id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        
        self.dataArr = [[NSMutableArray alloc] init];

    }
    
    return self;
}


-(void)pushSearchViewController
{

    WDDeviceSearchViewController *deviceSearchVC = [[WDDeviceSearchViewController alloc] initWithNibName:@"WDDeviceSearchViewController" bundle:nil];
    [self.navigationController pushViewController:deviceSearchVC animated:YES];
    
}

-(void)refreshDeviceListWith:(NSMutableArray *)deviceDataArr
{
    
    [self.dataArr removeAllObjects];
    [self.dataArr addObjectsFromArray:deviceDataArr];
    
    [self.tableView reloadData];
    
}

-(void)viewDidAppear:(BOOL)animated
{
    
    self.navigationController.navigationBarHidden = NO;
    self.navigationItem .title = @"智能彩灯";
    

    Wilddog *manageWilddog = [ApplicationDelegate.wilddog childByAppendingPath:@"devicemanage"];
    Wilddog *uidWilddog = [manageWilddog childByAppendingPath:[UserDefaults objectForKey:UID]];
    NSLog(@"[UserDefaults objectForKey:UID]: %@", [UserDefaults objectForKey:UID]);
    
    [Utils showProgressHUdWithText:@"正在获取设备..." showInView:self.view];
    [uidWilddog observeSingleEventOfType:WEventTypeValue withBlock:^(WDataSnapshot *snapshot) {
        
        [Utils hidenProgressHUdshowInView:self.view];
    
        NSDictionary *deviceDict = [[NSDictionary alloc] initWithDictionary:snapshot.value];

        NSLog(@"deviceDict: %@", deviceDict);
        
        NSMutableArray *deviceArr = [[NSMutableArray alloc] init];
        [deviceDict enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
            
            deviceItem *item = [[deviceItem alloc] init];
            item.deviceID = key;
            item.deviceBlue = [[obj objectForKey:@"led"] objectForKey:@"blue"];
            item.deviceRed = [[obj objectForKey:@"led"] objectForKey:@"red"];
            item.deviceGreen = [[obj objectForKey:@"led"] objectForKey:@"green"];
            item.deviceSW = [[obj objectForKey:@"led"] objectForKey:@"sw"];
            item.deviceNickName = [UserDefaults objectForKey:[NSString stringWithFormat:@"%ld%@",(unsigned long)[item.deviceID hash], item.deviceID]];
            
            [deviceArr addObject:item];
            
        }];
        //刷新数据
        [self refreshDeviceListWith:deviceArr];
        
    } withCancelBlock:^(NSError *error) {
        
        
        [Utils hidenProgressHUdshowInView:self.view];
        
        
        if (error) {
            [Utils showStatusTextWithText:@"获取设备列表失败" showInView:self.view];
        }
        
    }];
    
    
    
}


-(void)clickNavleftBt
{
    [self.sideMenuViewController presentLeftMenuViewController];
}

-(void)clickNavrightBt
{
    
    WDDeviceSearchViewController *deviceSearchVC = [[WDDeviceSearchViewController alloc] initWithNibName:@"WDDeviceSearchViewController" bundle:nil];
    [self.navigationController pushViewController:deviceSearchVC animated:YES];
    
}

-(void)setContentView
{
    self.navigationItem.leftBarButtonItem  = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"caidanlan"] style:UIBarButtonItemStylePlain target:self action:@selector(clickNavleftBt)];
    self.navigationItem.rightBarButtonItem  = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"tianjia"] style:UIBarButtonItemStylePlain target:self action:@selector(clickNavrightBt)];
    
    
    UIEdgeInsets inset = UIEdgeInsetsMake(44, 0, 0, 0);
    self.tableView.contentInset = inset;
    
    self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    self.tableView.backgroundColor = [UIColor colorWithHex:0xedf0f0];
    
    
    UIView *view = [[UIView alloc] initWithFrame:CGRectMake(0, -63, SCREEN_WIDTH, 63)];
    view.backgroundColor = [UIColor colorWithHex:0xffffff];
    
    UILabel *lab = [[UILabel alloc] initWithFrame:CGRectMake(0, 30, SCREEN_WIDTH, 20)];
    lab.text = @"设备列表";
    lab.textColor = [UIColor colorWithHex:0x7a7a7a];
    lab.textAlignment = NSTextAlignmentCenter;
    //lab.backgroundColor = [UIColor redColor];
    
    UIView *line = [[UIView alloc] initWithFrame:CGRectMake(0, 62, SCREEN_WIDTH, 1)];
    line.backgroundColor = [UIColor colorWithHex:0xe0e0e0];
    
    
    [view addSubview:lab];
    [view addSubview:line];
    
    [self.view addSubview:view];

}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self setContentView];

}


#pragma mark * Table view data source


-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    [self.tableView deselectRowAtIndexPath:indexPath animated:NO];
    
    WDDeviceShowViewController *deviceManagerViewController = [[WDDeviceShowViewController alloc] initWithNibName:@"WDDeviceShowViewController" bundle:nil];

    deviceManagerViewController.mDeviceItem = [self.dataArr objectAtIndex:indexPath.row];
   
    [self.navigationController pushViewController:deviceManagerViewController animated:YES];
    
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.dataArr.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    WDDeviceCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    
    if (!cell) {
        
        cell = [[[NSBundle mainBundle] loadNibNamed:@"WDDeviceCell" owner:nil options:nil] lastObject];
        cell.backgroundColor = [UIColor colorWithHex:0xedf0f0];
    }
    
    deviceItem *item = [self.dataArr objectAtIndex:indexPath.row];
    cell.mDeviceNameLab.text = [item showDeviceName];
    
    cell.delegate = self;
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 95.;
}

#pragma mark * DAContextMenuCell delegate

- (void)contextMenuCellDidSelectDeleteOption:(DAContextMenuCell *)cell
{
    
    deviceItem *item = [self.dataArr objectAtIndex:[self.tableView indexPathForCell:cell].row];
    
    NSString *deviceName = [NSString stringWithFormat:@"设备名: %@",item.deviceID];
    
    AlertBlock *alert = [[AlertBlock alloc] initWithTitle:@"删除设备" message:deviceName cancelButtonTitle:@"取消" otherButtonTitles:@"确定" block:^(NSInteger btnIndex) {
        
        if (1 == btnIndex) {
            
            Wilddog *manageWilddog = [ApplicationDelegate.wilddog childByAppendingPath:@"devicemanage"];
            Wilddog *uidWilddog = [manageWilddog childByAppendingPath:[UserDefaults objectForKey:UID]];
            Wilddog *deviceIdWilddog = [uidWilddog childByAppendingPath:item.deviceID];
            
            
            [deviceIdWilddog removeValueWithCompletionBlock:^(NSError *error, Wilddog *ref) {
                
                
                if (!error) {
                    
                    [self.sideMenuViewController hideMenuViewController];
                    
                    [super contextMenuCellDidSelectDeleteOption:cell];
                    [self.dataArr removeObject:item];
                    
                    [self.tableView beginUpdates];
                    [self.tableView deleteRowsAtIndexPaths:@[[self.tableView indexPathForCell:cell]] withRowAnimation:UITableViewRowAnimationAutomatic];
                    [self.tableView endUpdates];
                    
                    [UserDefaults removeObjectForKey:[NSString stringWithFormat:@"%ld%@",[item.deviceID hash], item.deviceID]];
                    
                }else {
                    
                    
                    
                    [Utils hidenProgressHUdshowInView:self.view];
                    [Utils showStatusTextWithText:@"删除失败" showInView:self.view];
                }
                
            }];
        }
    }];
    
    [alert show];
    
}


- (void)contextMenuCellDidSelectMoreOption:(DAContextMenuCell *)cell
{
    
    deviceItem *item = [self.dataArr objectAtIndex:[self.tableView indexPathForCell:cell].row];
    
    NSString *deviceName = [NSString stringWithFormat:@"设备名: %@",[item showDeviceName]];
    
    
    AlertBlock *alert = [[AlertBlock alloc] initWithTitle:@"设备改名" message:deviceName cancelButtonTitle:@"取消" otherButtonTitles:@"确定" Inputblock:^(NSInteger btnIndex, NSString *input) {
        
        if (1 == btnIndex) {
            
            item.deviceNickName = input;
            [self.tableView reloadData];
            
            [UserDefaults setObject:input forKey:[NSString stringWithFormat:@"%ld%@",[item.deviceID hash], item.deviceID]];
            [UserDefaults synchronize];
        }
    }];
    
    alert.alertViewStyle = UIAlertViewStylePlainTextInput;
    
    [alert show];

}

@end

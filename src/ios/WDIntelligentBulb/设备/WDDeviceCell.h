//
//  WDDeviceCell.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/7.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "DAContextMenuCell.h"

@interface WDDeviceCell : DAContextMenuCell

@property (weak, nonatomic) IBOutlet UIImageView *stateImageView;
@property (weak, nonatomic) IBOutlet UILabel *mDeviceNameLab;
@end

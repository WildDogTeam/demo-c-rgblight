//
//  UIColor+Util.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/12.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIColor (Util)

+ (UIColor *)colorWithHex:(int)hexValue alpha:(CGFloat)alpha;
+ (UIColor *)colorWithHex:(int)hexValue;

@end

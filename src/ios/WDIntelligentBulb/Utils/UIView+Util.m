//
//  UIView+Util.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/12.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "UIView+Util.h"

@implementation UIView (Util)

- (void)setCornerRadius:(CGFloat)cornerRadius
{
    self.layer.cornerRadius = cornerRadius;
    self.layer.masksToBounds = YES;
}

- (void)setBorderWidth:(CGFloat)width andColor:(UIColor *)color
{
    self.layer.borderWidth = width;
    self.layer.borderColor = color.CGColor;
}


@end

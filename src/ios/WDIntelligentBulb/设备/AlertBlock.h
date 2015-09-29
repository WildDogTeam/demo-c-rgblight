//
//  AlertBlock.h
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/21.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import <UIKit/UIKit.h>

@class AlertBlock;
typedef void (^TouchBlock)(NSInteger btnIndex);

typedef void (^TouchBlockInput)(NSInteger btnIndex,NSString *input);

@interface AlertBlock : UIAlertView

@property(nonatomic,copy)TouchBlock block;

@property(nonatomic,copy)TouchBlockInput blockInput;


//需要自定义初始化方法，调用Block
- (id)initWithTitle:(NSString *)title
            message:(NSString *)message
  cancelButtonTitle:(NSString *)cancelButtonTitle
  otherButtonTitles:(NSString*)otherButtonTitles
              Inputblock:(TouchBlockInput)block;


//需要自定义初始化方法，调用Block
- (id)initWithTitle:(NSString *)title
            message:(NSString *)message
  cancelButtonTitle:(NSString *)cancelButtonTitle
  otherButtonTitles:(NSString*)otherButtonTitles
              block:(TouchBlock)block;
@end

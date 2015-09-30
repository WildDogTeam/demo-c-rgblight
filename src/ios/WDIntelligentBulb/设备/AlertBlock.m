//
//  AlertBlock.m
//  WDIntelligentBulb
//
//  Created by IMacLi on 15/8/21.
//  Copyright (c) 2015年 liwuyang. All rights reserved.
//

#import "AlertBlock.h"

@implementation AlertBlock

- (id)initWithTitle:(NSString *)title message:(NSString *)message cancelButtonTitle:(NSString *)cancelButtonTitle otherButtonTitles:(NSString*)otherButtonTitles block:(TouchBlock)block{
    self = [super initWithTitle:title message:message delegate:self cancelButtonTitle:cancelButtonTitle otherButtonTitles:otherButtonTitles, nil];//注意这里初始化父类的
    if (self) {
        self.block = block;
    }
    return self;
}

//#pragma mark -AlertViewDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
    //这里调用函数指针_block(要传进来的参数);
    
    
    if (_block) {
        _block(buttonIndex);
    }
    
    
    
    if (_blockInput) {
        
        //得到输入框
        UITextField *tf=[alertView textFieldAtIndex:0];
        _blockInput(buttonIndex,tf.text);
        
    }
   
    
}



-(id)initWithTitle:(NSString *)title message:(NSString *)message cancelButtonTitle:(NSString *)cancelButtonTitle otherButtonTitles:(NSString *)otherButtonTitles Inputblock:(TouchBlockInput)block
{

    self = [super initWithTitle:title message:message delegate:self cancelButtonTitle:cancelButtonTitle otherButtonTitles:otherButtonTitles, nil];//注意这里初始化父类的
    

    if (self) {
        
        self.blockInput = block;
    
        
    }
    return self;
    
}


@end

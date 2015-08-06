/*******************************************************************************
 The MIT License (MIT)

 Copyright (c) 2015 Alexander Zolotarev <me@alex.bio> from Minsk, Belarus

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 *******************************************************************************/

// Convenience header to use from pure Objective-C project.
// TODO(AlexZ): Refactor it to use instance and it's methods instead of static functions.

#ifndef ALOHALYTICS_OBJC_H
#define ALOHALYTICS_OBJC_H

#import <Foundation/Foundation.h>
#import <Foundation/NSDate.h>
#import <CoreLocation/CoreLocation.h>

@interface Alohalytics : NSObject
+ (void)setDebugMode:(BOOL)enable;
// Should be called in application:didFinishLaunchingWithOptions: or in application:willFinishLaunchingWithOptions:
// Final serverUrl is modified to $(serverUrl)/[ios|mac]/your.bundle.id/app.version
+ (void)setup:(NSString *)serverUrl withLaunchOptions:(NSDictionary *)options;
// Alternative to the previous setup method if you integrated Alohalytics after initial release
// and don't want to count app upgrades as new installs (and definitely know that it's an already existing user).
+ (void)setup:(NSString *)serverUrl andFirstLaunch:(BOOL)isFirstLaunch withLaunchOptions:(NSDictionary *)options;
+ (void)forceUpload;
+ (void)logEvent:(NSString *)event;
+ (void)logEvent:(NSString *)event atLocation:(CLLocation *)location;
+ (void)logEvent:(NSString *)event withValue:(NSString *)value;
+ (void)logEvent:(NSString *)event withValue:(NSString *)value atLocation:(CLLocation *)location;
// Two convenience methods to log [key1,value1,key2,value2,...] arrays.
+ (void)logEvent:(NSString *)event withKeyValueArray:(NSArray *)array;
+ (void)logEvent:(NSString *)event withKeyValueArray:(NSArray *)array atLocation:(CLLocation *)location;
+ (void)logEvent:(NSString *)event withDictionary:(NSDictionary *)dictionary;
+ (void)logEvent:(NSString *)event withDictionary:(NSDictionary *)dictionary atLocation:(CLLocation *)location;
// Returns the date when app was launched for the first time (usually the same as install date).
+ (NSDate *)firstLaunchDate;
// Returns summary time of all active user sessions up to now.
+ (NSInteger)totalSecondsSpentInTheApp;
@end

#endif  // #ifndef ALOHALYTICS_OBJC_H

//
//  AstrolabeCompendium.m
//  AstrolabeCompendium
//
//  Created by Ian Ollmann on 7/1/24.
//

#import "AstrolabeCompendium.h"

#ifndef __has_feature         // Optional of course.
  #define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
  #define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif
#ifndef __has_attribute         // Optional of course.
  #define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif

#if __has_feature(objc_arc)
#   define IF_NOT_ARC(_a)
#   define IF_ARC(_a)      (_a)
#   define RELEASE(_a)     {(_a) = nil;}
#   define RETAIN(_a)      ({ __typeof__(_a) __a = _a; /*return*/ __a;})
#   define AUTORELEASE(_a) ({ __typeof__(_a) __a = _a; /*return*/ __a;})
#else
#   define IF_NOT_ARC(_a)  (_a)
#   define IF_ARC(_a)
#   define RELEASE(_a)     [(_a) release]
#   define RETAIN(_a)      [(_a) retain]
#   define AUTORELEASE(_a) [(_a) autorelease]
#endif

static NSString * MechanusDataTypeString = @"T";
static NSString * MechanusDataDataString = @"D";
static NSString * AstrolabeCompendiumErrorDomain = @"AstrolabeCompendiumErrorDomain";
static NSString * kTemplateKey = @"<template>";
static NSString * kToolTipsKey = @"<ToolTips>";
@interface MechanusImage : NSImage
@property (nonnull, nonatomic, strong) NSData * data;
- (nullable instancetype) initWithData: (NSData*__nonnull) data;
@end

BOOL KeyIsMetadata(id __nullable key)
{
    if( NO == [key isKindOfClass: NSString.class] )
        return NO;
    
    NSString * __nonnull k = (NSString *) key;
    return [k hasPrefix: @"<"] && [k hasSuffix: @">"];
}


@implementation MechanusImage
@synthesize data;

-(void) dealloc{ RELEASE(data); IF_NOT_ARC([super dealloc]); }
-(nullable instancetype) initWithData: (NSData*__nonnull) data
{
    self = [super initWithData: data];
    if(nil == self)
        return self;
    
    self.data = data;
    return self;
}
@end


#pragma mark -

@interface MechanusData : NSObject <NSSecureCoding>
@property (nonnull, nonatomic, readwrite, strong) NSObject<NSSecureCoding> * object;
+ (nullable instancetype) mechanusDataWithObject:  (NSObject<NSSecureCoding> *) object;
- (nullable instancetype) initWithCoder: (NSCoder*__nonnull) coder;
- (void) encodeWithCoder: (NSCoder*__nonnull) coder;
- (NSData * __nullable) dataRepresentationWithError: (NSError* __nullable __autoreleasing * __nonnull) error;
@end

@implementation MechanusData
@synthesize  object;

-(void) dealloc { RELEASE(object); IF_NOT_ARC([super dealloc]); }

+ (nullable instancetype) mechanusDataWithObject: (NSObject<NSSecureCoding> *) object
{
    MechanusData * result = [MechanusData new];
    result.object = object;
    return AUTORELEASE(result);
}

+ (BOOL) supportsSecureCoding{ return YES;}
- (instancetype) initWithCoder: (NSCoder*) coder
{
    NSString * className = [coder decodeObjectOfClass: NSString.class forKey: MechanusDataTypeString ];
    NSData * data = [coder decodeObjectOfClass: NSData.class forKey: MechanusDataDataString ];
    if( nil == className || nil == data)
    {
        self = nil;
        return self;
    }
    
    self = [super init];
    if( nil == self)
        return self;
    

    if( [className hasSuffix: @"AttributedString"])
        self.object = AUTORELEASE( [[NSMutableAttributedString alloc] initWithRTFD: data
                                                                documentAttributes: nil] );
    else if ( [className hasSuffix: @"Image"])
        self.object = AUTORELEASE( [[MechanusImage alloc] initWithData: data] );
    else
    {
        IF_NOT_ARC( [self dealloc] );
        self = nil;
    }
    
    return self;
}

- (void) encodeWithCoder: (NSCoder*) coder
{
    [coder encodeObject: NSStringFromClass( self.object.class)
                 forKey: MechanusDataTypeString ];
    
    if( [self.object isKindOfClass: MechanusImage.class] )
        [coder encodeObject: [(MechanusImage*) self.object data]
                     forKey: MechanusDataDataString ];
    else if( [self.object isKindOfClass: NSAttributedString.class] )
    {
        NSAttributedString * attrString = (NSAttributedString*) self.object;
        NSData * data = [attrString RTFDFromRange: NSMakeRange(0, attrString.length) documentAttributes: @{}];
        [coder encodeObject: data forKey: MechanusDataDataString ];
    }
    else
        assert(false);  // unhandled object type
}

- (NSData*) dataRepresentationWithError: (NSError * __nullable __autoreleasing * __nonnull) error
{
    return  [NSKeyedArchiver archivedDataWithRootObject: self
                                  requiringSecureCoding: YES
                                                  error: error];
}

@end


#pragma mark -

static NSObject * __nullable
    CanonicalizeObject( NSObject* __nonnull object,
                        NSObject* __nullable templateObject,
                        NSString * __nonnull name,
                        NSError * __nullable __autoreleasing * __nonnull error ) NS_RETURNS_NOT_RETAINED;


static inline NSError * __nullable
    MakeError( NSString * __nonnull message, NSString * objectName, NSObject * __nullable o) NS_RETURNS_NOT_RETAINED
    {
        return [NSError errorWithDomain: AstrolabeCompendiumErrorDomain
                                   code: 0
                               userInfo: @{ NSLocalizedDescriptionKey : @"Input file error: object should be a dictionary. Replacing content with correct empty type.",
                                            NSLocalizedFailureReasonErrorKey : o.debugDescription,
                                            NSLocalizedRecoverySuggestionErrorKey : [NSString stringWithFormat: @"Object %@ <%p> is a %@", objectName, o, NSStringFromClass( o.class)]
                                         }];
    }

// might be NSMutableDictionary or NSMutableArray <NSMutableDictionary*>
static NSObject * __nullable
    ConvertToMutableDictionary( NSObject * __nullable o,
                                NSDictionary * templateDictionary,
                                NSString * objectName,
                                NSError * __nullable __autoreleasing * error ) NS_RETURNS_NOT_RETAINED
{
    if( NO == [templateDictionary isKindOfClass: NSDictionary.class])
        templateDictionary = nil;
    
    if( [o isKindOfClass: NSDictionary.class])
    {
        NSMutableDictionary *d = (NSMutableDictionary*) o;
        if( NO == [d isKindOfClass: NSMutableDictionary.class])
            d = AUTORELEASE( [d mutableCopy]);
        
        __block NSError * e = nil;
        if( nil == templateDictionary)
        {
            NSDictionary * childTemplateDictionary = d[kTemplateKey];
            [d enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull original, BOOL * _Nonnull stop)
            {
                NSObject * o = CanonicalizeObject( original, childTemplateDictionary, key, &e);
                if( o && o != original)
                    d[key] = o;
                RETAIN(e);
            }];
        }
        else
        {
            BUG there are sub dictionaries with named items in them of indeterminate name, and this
                  approach here doesn't work for that. We need some naming convention for anyname
            [templateDictionary enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull templateObject, BOOL * _Nonnull stop)
            {
                if( [key isEqualToString: @"Attacks"])
                    printf("foo");
                NSObject * original = d[key];
                NSObject * o = original || !KeyIsMetadata(key) ? original : templateObject;
                o = CanonicalizeObject( o, templateDictionary[key], key, &e);
                if( o && o != original)
                    d[key] = o;
                RETAIN(e);
            }];
        }
        if(e)
        {
            *error = e;
            AUTORELEASE(e);
        }
        return (NSMutableDictionary*) d;
    }

    if( [o isKindOfClass: NSArray.class ] )
    {
        NSMutableArray * array = (NSMutableArray*) o;
        if( NO == [array isKindOfClass: NSMutableArray.class])
            array = AUTORELEASE(array.mutableCopy);
        
        unsigned long count = array.count;
        for( unsigned long i = 0; i < count; i++)
        {
            NSObject * o = array[i];
            NSString * name = [NSString stringWithFormat: @"%@[%lu]", objectName, i];
            NSObject * d = CanonicalizeObject( o, o, name, error);
            if( d != o)
                array[i] = d;
        }
        
        return array;
    }

    if( [o isKindOfClass: NSNumber.class] ||
        [o isKindOfClass: NSString.class])
    {
        if( nil == *error )
            *error = MakeError( @"Input file error: object should be a dictionary. Replacing content with correct empty type.", objectName, o);
                                            
        return [NSMutableDictionary dictionaryWithDictionary: @{}];
    }

    if( nil == *error )
        *error = MakeError( @"Failed to convert object to a dictionary", objectName, o);

    return nil;
}

// might be NSMutableString or NSMutableArray <NSMutableString*>
static NSObject * __nullable
    ConvertToMutableString( NSObject * __nullable o,
                            NSString * __nonnull objectName,
                            NSError * __nullable __autoreleasing * error ) NS_RETURNS_NOT_RETAINED
{
    if( YES == [o isKindOfClass: NSMutableString.class])
        return (NSMutableString*) o;
    
    if( [o isKindOfClass: NSString.class])
        return AUTORELEASE([o mutableCopy]);
    
    if( [o isKindOfClass: NSAttributedString.class])
        return [NSMutableString stringWithString: [(NSAttributedString*) o string]];
    
    if( [o isKindOfClass: NSNumber.class] )
       return AUTORELEASE( [(NSNumber*)o stringValue].mutableCopy );
    
    if( [o isKindOfClass: NSArray.class ] )
    {
        NSMutableArray * array = (NSMutableArray*) o;
        if( NO == [array isKindOfClass: NSMutableArray.class])
            array = AUTORELEASE(array.mutableCopy);
        
        unsigned long count = array.count;
        for( unsigned long i = 0; i < count; i++)
        {
            NSString * name = [NSString stringWithFormat: @"%@[%lu]", objectName, i];
            NSObject * o = array[i];
            NSObject * d = ConvertToMutableString( CanonicalizeObject(o, o, name, error),
                                                   name,
                                                   error );
            if( d != o)
                array[i] = d;
        }
        
        return array;
    }

    if( nil == *error )
        *error = MakeError( @"Failed to convert object to a string", objectName, o);
    return nil;
}

static NSObject * __nullable
    ConvertToMutableAttributedString( NSObject * __nullable o,
                                      NSString * __nonnull objectName,
                                      NSError * __nullable __autoreleasing * error ) NS_RETURNS_NOT_RETAINED
{
    if( [o isKindOfClass: NSAttributedString.class])
    {
        if( [o isKindOfClass: NSMutableAttributedString.class])
            return (NSMutableAttributedString*) o;
        
        return AUTORELEASE( [o mutableCopy] );
    }
    
    if( [o isKindOfClass: NSString.class])
        return AUTORELEASE( [[NSMutableAttributedString alloc] initWithString: (NSString*) o ] );
    
    if( [o isKindOfClass: NSArray.class ] )
    {
        NSMutableArray * array = (NSMutableArray*) o;
        if( NO == [array isKindOfClass: NSMutableArray.class])
            array = AUTORELEASE( array.mutableCopy );
        
        unsigned long count = array.count;
        for( unsigned long i = 0; i < count; i++)
        {
            NSString * name = [NSString stringWithFormat: @"%@[%lu]", objectName, i];
            NSObject * o = array[i];
            NSObject * d = ConvertToMutableAttributedString( CanonicalizeObject(o, o, name, error),
                                                             name,
                                                             error );
            if( d != o)
                array[i] = d;
        }
        
        return array;
    }
    
    if( [o isKindOfClass: NSDictionary.class])
    {
        if( nil == *error )
            *error = MakeError( @"Input file error: object should be a formatted string. Replacing content.", objectName, o);

        return AUTORELEASE( [NSMutableAttributedString new] );
    }
    
    if( nil == *error )
        *error = MakeError( @"Failed to convert object to a formatted string", objectName, o);
    return nil;
}

static NSMutableArray * __nullable
    ConvertToMutableArray( NSObject * __nullable o,
                           NSString * __nonnull objectName,
                           NSError * __nullable __autoreleasing * __nonnull error ) NS_RETURNS_NOT_RETAINED
{
    if( YES == [o isKindOfClass: NSMutableArray.class])
        return (NSMutableArray*) o;
    
    if( YES == [o isKindOfClass: NSArray.class])
        return AUTORELEASE( o.mutableCopy );
    
    if( [o isKindOfClass: NSNumber.class] )
        return [NSMutableArray arrayWithObject: o];

    if( [o isKindOfClass: NSString.class])
        return [NSMutableArray arrayWithObject: [NSMutableString stringWithString: (NSString*) o]];
        
    if( [o isKindOfClass: NSDictionary.class])
    {
        NSMutableArray * a = AUTORELEASE( [NSMutableArray new] );
        __block unsigned long index = 0;
        __block NSError * e = nil;
        [(NSDictionary*)o enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop) {
            NSString * name = [NSString stringWithFormat: @"%@[%lu]", objectName, index];
            a[index++] = CanonicalizeObject(obj, obj, name, &e);
            RETAIN(e);
        }];
        
        if(e)
        {
            AUTORELEASE(e);
            *error = e;
        }
        return a;
    }

    if( nil == *error )
        *error = MakeError( @"Failed to convert object to an array.", objectName, o);
    return nil;
}

static NSObject * __nullable
    ConvertToNumber( NSObject * __nullable o,
                     NSString* objectName,
                     NSError * __nullable __autoreleasing * __nonnull error ) NS_RETURNS_NOT_RETAINED
{
    if( [o isKindOfClass: NSNumber.class])
        return (NSNumber*) o;
    
    if( [o isKindOfClass: NSString.class])
        return [NSNumber numberWithDouble: [(NSString*) o doubleValue] ];
    
    if( [o isKindOfClass: NSArray.class ] )
    {
        NSMutableArray * array = (NSMutableArray*) o;
        if( NO == [array isKindOfClass: NSMutableArray.class])
            array = AUTORELEASE( array.mutableCopy );
        
        unsigned long count = array.count;
        for( unsigned long i = 0; i < count; i++)
        {
            NSString * name = [NSString stringWithFormat: @"%@[%lu]", objectName, i];
            NSObject * o = array[i];
            NSObject * d = ConvertToNumber( CanonicalizeObject(o, o, name, error),
                                            name,
                                            error );
            if( d != o)
                array[i] = d;
        }
        
        return array;
    }
    
    if( [o isKindOfClass: NSDictionary.class])
    {
        if( nil == *error)
            *error = MakeError( @"Input file error: object should be a number. Replacing content.", objectName, o);
        return  [NSNumber numberWithDouble: 0];
    }
    
    if( nil == *error)
        *error = MakeError( @"Failed to convert object to a number.", objectName, o);
    return nil;
}

static NSObject *
    ConvertToImage(NSObject * __nullable o,
                   NSString * __nonnull objectName,
                   NSError * __nullable __autoreleasing * __nonnull error ) NS_RETURNS_NOT_RETAINED
{
    if( [o isKindOfClass: NSImage.class])
        return (NSImage*) o;
    
    if([o isKindOfClass: NSData.class])
    {
        if( [o isKindOfClass: NSData.class])
        {
            o = [NSKeyedUnarchiver unarchivedObjectOfClass: MechanusData.class
                                                  fromData: (NSData*) o
                                                     error: error ];

            if( error )
                return nil;
        }
        
        if( [o isKindOfClass: MechanusData.class])
            o = [(MechanusData*)o object];
        
        if( [o isKindOfClass: NSImage.class])
            return o;
        
        if( nil == *error)
            *error = MakeError( @"Error: attempted to decode MechanusData object to image, but got something else instead.\n", objectName, o);
        return nil;
    }
    
    if( [o isKindOfClass: NSArray.class ] )
    {
        NSArray * a = (NSArray*) o;
        NSMutableArray * array;
        if( [a isKindOfClass: NSMutableArray.class])
            array = (NSMutableArray*) a;
        else
            array = [NSMutableArray arrayWithArray: a];
        
        unsigned long count = array.count;
        for( unsigned long i = 0; i < count; i++)
        {
            NSString * name = [NSString stringWithFormat: @"%@[%lu]", objectName, i];
            NSObject * o = array[i];
            NSObject * d = ConvertToImage( CanonicalizeObject(o, o, name, error),
                                           name,
                                           error );
            ;
            if( d != o)
                array[i] = d;
        }
        
        return array;
    }

    if( nil == *error)
        *error = MakeError( @"Failed to convert object to an image.", objectName, o);
    return nil;
}


static inline bool IsKindOfAttributedString(NSObject * __nullable o)
{
    if( nil == o)
        return false;
    
    if( [o isKindOfClass: NSAttributedString.class])
        return true;
    
    if( [o isKindOfClass: MechanusData.class] )
        return [[(MechanusData*) o object] isKindOfClass: NSAttributedString.class ];

    return false;
}


static inline bool IsKindOfString(NSObject * __nullable o)
{
    if( nil == o)
        return false;
    
    if( [o isKindOfClass: NSString.class ])
        return true;
    
    if( IsKindOfAttributedString(o))
        return true;
    
    return false;
}

static inline bool IsKindOfArray(NSObject * __nullable o) { return [o isKindOfClass: NSArray.class];}
static inline bool IsKindOfDictionary(NSObject * __nullable o) { return [o isKindOfClass: NSDictionary.class];}
static inline bool IsKindOfNumber(NSObject * __nullable o) { return [o isKindOfClass: NSNumber.class];}
static inline bool IsKindOfImage(NSObject * __nullable o)
{
    if( NO == [o isKindOfClass: MechanusData.class])
        return NO;
    
    return [[(MechanusData*) o object] isKindOfClass: NSImage.class];
}

static inline bool IsKindOrArrayOfKind( bool (*isKindOf)(NSObject * o), NSObject * __nullable o)
{
    if( [o isKindOfClass: NSArray.class])
    {
        NSArray * array = (NSArray*) o;
        unsigned long count = array.count;
        for( unsigned long i = 0; i < count; i++ )
            if( ! isKindOf(array[i]))
                return NO;
                
        return YES;
    }
    
    return isKindOf(o);
}


// Convert an input file object to something useful in a NSDictionary
static NSObject * __nullable 
    CanonicalizeObject( NSObject* __nonnull object,
                        NSObject* __nullable templateObject,
                        NSString * __nonnull name,
                        NSError * __nullable __autoreleasing * __nonnull error) NS_RETURNS_NOT_RETAINED
{
    NSObject * o = object;
    if( [object isKindOfClass: NSData.class])
    {
        MechanusData * data = [NSKeyedUnarchiver unarchivedObjectOfClass: MechanusData.class
                                                                fromData: (NSData*) object
                                                                   error: error];
        
        if( nil == *error )
            o = data.object;
        return o;
    }

    NSObject * prototype = templateObject ? templateObject : o;
    if( [name isEqualToString: @"Description"] ||
        (IsKindOrArrayOfKind(IsKindOfString, prototype) &&
         IsKindOrArrayOfKind(IsKindOfAttributedString, o)))
        o = ConvertToMutableAttributedString( o, name, error);
    else if( IsKindOrArrayOfKind(IsKindOfString, prototype))
        o = ConvertToMutableString( o, name, error);
    else if( IsKindOrArrayOfKind(IsKindOfDictionary,  prototype))
        o = ConvertToMutableDictionary( o, (NSDictionary*) templateObject, name, error);
    else if( IsKindOrArrayOfKind(IsKindOfArray, prototype))
        o = ConvertToMutableArray( o, name, error);
    else if( IsKindOrArrayOfKind(IsKindOfNumber, prototype))
        o = ConvertToNumber( o, name, error);
    else if( IsKindOrArrayOfKind(IsKindOfImage, prototype))
        o = ConvertToImage( o, name, error);
    else
    {
        NSString * errorMessage;
        if( templateObject)
            errorMessage = [NSString stringWithFormat: @"Input file error: template field %@ has unhandled type %@n", name, NSStringFromClass([templateObject class])];
        else
            errorMessage = [NSString stringWithFormat: @"Input file error: object field %@ has unhandled type %@n", name, NSStringFromClass([prototype class])];

        if( nil == *error)
            *error = MakeError( errorMessage, name, o);

        return nil;
    }
        
    return o;
}


MechanusImage *
    AstrolabeCompendiumCreateImageFromURL( NSURL * __nullable url,
                                           NSError * __nullable __autoreleasing * error) NS_RETURNS_RETAINED
{
    NSError * e = nil;
    MechanusImage * result = nil;
    @autoreleasepool {
        NSData * data = [NSData dataWithContentsOfURL: url
                                              options: 0
                                                error: &e];
        if( nil == data)
            return nil;
            
        result = (MechanusImage*) AstrolabeCompendiumCreateImageFromData(data);
        RETAIN(e);
    }
    
    if(e)
    {
        AUTORELEASE(e);
        *error = e;
    }
    return result;
}

NSImage *
AstrolabeCompendiumCreateImageFromData( NSData * __nullable data) NS_RETURNS_RETAINED
{
    return [[MechanusImage alloc] initWithData: data];
}

#pragma mark -

NSMutableDictionary * __nullable AstrolabeCompendiumLoadFromURL( NSURL * __nullable url,
                                                                 NSError * __nullable __autoreleasing * __nonnull errorOut ) NS_RETURNS_RETAINED
{
    NSMutableDictionary * result = nil;
    NSError * e = nil;
    @autoreleasepool {
        result = (NSMutableDictionary*) CanonicalizeObject( [NSDictionary dictionaryWithContentsOfURL: url], nil, @"<root>", &e);
        RETAIN(result);

        assert( [result isKindOfClass: NSMutableDictionary.class] );
        RETAIN(e);
    }
    if(e)
    {
        AUTORELEASE(e);
        *errorOut = e;
    }
    return result;
}



#pragma mark -

NSMutableArray * FormatArrayForStorage( NSArray * d,
                                        NSString * name,
                                        NSError * __nullable __autoreleasing * __nonnull error) NS_RETURNS_RETAINED;

NSMutableDictionary * FormatDictionaryForStorage( NSDictionary * d,
                                                  NSError * __nullable __autoreleasing * __nonnull error) NS_RETURNS_RETAINED
{
    NSMutableDictionary * saved = [NSMutableDictionary new];
    __block NSError * e = nil;
    
    @autoreleasepool {
        // copy out plist entries, converting to MechanusData as needed as we go
        [d enumerateKeysAndObjectsUsingBlock:^(id  _Nonnull key, id  _Nonnull obj, BOOL * _Nonnull stop)
         {
            if( [obj isKindOfClass: NSString.class ]       ||
               [obj isKindOfClass: NSNumber.class ]       ||
               [obj isKindOfClass: NSDate.class ] )
            {
                saved[key] = obj;
                return;
            }
            
            if([obj isKindOfClass: NSDictionary.class ])
            {
                NSDictionary * formatted = FormatDictionaryForStorage(obj, &e);
                saved[key] = formatted;
                RELEASE(formatted);
                return;
            }
            
            if([obj isKindOfClass: NSArray.class ])
            {
                NSMutableArray * formatted = FormatArrayForStorage(obj, key, &e);
                saved[key] = formatted;
                RELEASE(formatted);
                return;
            }
            
            if( [obj isKindOfClass: MechanusImage.class] ||
               [obj isKindOfClass: NSAttributedString.class])
            {
                saved[key] = [[MechanusData mechanusDataWithObject: obj] dataRepresentationWithError: &e];
                return;
            }
            
            if( [obj isKindOfClass: NSImage.class])
            {
                e = MakeError( @"Image was not created with AstrolabeCompendiumCreateImageFromData/URL() and could not be saved.", key, obj);
                return;
            }
            
            e = MakeError( @"Invalid object type in dictionary could not be saved to plist.", key, obj);
        }];
        RETAIN(e);
    };

    if(e)
    {
        AUTORELEASE(e);
        *error = e;
    }
    return saved;
}

NSMutableArray * FormatArrayForStorage( NSArray * a,
                                        NSString * name,
                                        NSError * __nullable __autoreleasing * __nonnull errorOut) NS_RETURNS_RETAINED
{
    NSMutableArray * result = [[NSMutableArray alloc] initWithCapacity: a.count];
    NSError * e = nil;
    @autoreleasepool
    {
        unsigned long count = a.count;
        for( unsigned long i = 0; i < count; i++)
        {
            NSObject * obj = a[i];
            
            if( [obj isKindOfClass: NSString.class ]       ||
                [obj isKindOfClass: NSNumber.class ]       ||
                [obj isKindOfClass: NSDate.class ] )
            {
                result[i] = obj;
                continue;
            }
            
            if([obj isKindOfClass: NSDictionary.class ])
            {
                NSMutableDictionary * formatted = FormatDictionaryForStorage((NSDictionary*) obj, &e);
                result[i] = formatted;
                RELEASE(formatted);
                continue;
            }

            if([obj isKindOfClass: NSArray.class ])
            {
                NSMutableArray * array = FormatArrayForStorage((NSArray*) obj, name, &e);
                result[i] = array;
                RELEASE(array);
                continue;
            }

            if( [obj isKindOfClass: MechanusImage.class] ||
                [obj isKindOfClass: NSAttributedString.class])
            {
                result[i] = [[MechanusData mechanusDataWithObject: (NSObject<NSSecureCoding>*)obj] dataRepresentationWithError: &e];
                continue;
            }
            
            NSString * objName = [NSString stringWithFormat: @"%@[%lu]", name, i];
            if( [obj isKindOfClass: NSImage.class])
            {
                e = MakeError( @"Image was not created with AstrolabeCompendiumCreateImageFromData/URL() and could not be saved.", objName, obj);
                continue;
            }
            
            e = MakeError( @"Invalid object type in array could not be saved to plist.", objName, obj);
        }
        RETAIN(e);
    }
    if(e)
    {
        AUTORELEASE(e);
        *errorOut = e;
    }
    
    return result;
}


void AstrolabeCompendiumWriteToURL( NSDictionary * __nullable compendium,
                                    NSURL * __nullable url,
                                    NSError * __nullable __autoreleasing * __nonnull errorOut )
{
    NSDictionary * saved = FormatDictionaryForStorage( compendium, errorOut);

    [saved writeToURL: url  
                error: errorOut];

    RELEASE(saved);
}




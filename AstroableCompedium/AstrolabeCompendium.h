//
//  AstrolabeCompendium.h
//  AstrolabeCompendium
//
//  Created by Ian Ollmann on 7/1/24.
//

#ifndef AstrolabeCompendium_h
#define AstrolabeCompendium_h

#include <Availability.h>


#ifdef __cplusplus
extern "C"  {
#endif /* __cplusplus*/


#ifdef __OBJC__

#if defined __MAC_OS_X_VERSION_MIN_REQUIRED
#   import <AppKit/AppKit.h>
#   define ASTROLABE_COMPENDIUM_TARGET_MAC 1
#else
#   import <UIKit/UIKit.h>
#   define ASTROLABE_COMPENDIUM_TARGET_IOS 1
#endif

// Version number   major.minor.bugfix  16.8.8
#define ASTROLABE_COMPENDIUM_VERSION    0x10000U        /* 1.0.0 */

#define ASTROLABE_EXTERN extern

/*! @abstract Load a compendium as a URL
 *  @param url      Where to save the compendium. If nil, then nothing happens
 *  @param errorOut  On return, the address of a NSError containing failure information. Otherwise unmodified.
 *  @return On success a non-nil NSDictionary will be returned. Otherwise nil will be returned and a NSError will be written to errorOut[0]  */
ASTROLABE_EXTERN NSMutableDictionary * __nullable
    AstrolabeCompendiumLoadFromURL( NSURL * __nullable url,
                                    NSError * __nullable __autoreleasing * __nonnull errorOut ) NS_RETURNS_RETAINED;

/*! @abstract Save a compendium to a URL
 *  @param compendium   A dictionary containing compendium and template/tootips data to save
 *  @param url  The location to save the compendium 
 *  @param errorOut  On return, the address of a NSError containing failure information. Otherwise unmodified. */
ASTROLABE_EXTERN void
    AstrolabeCompendiumWriteToURL( NSDictionary * __nullable compendium,
                                   NSURL * __nullable url,
                                   NSError * __nullable __autoreleasing * __nonnull errorOut );

/*! @abstract Create an image from a URL suitable for saving to a AstrolabeCompendium without loss
 *  @param url      A url where to find the image
 *  @param error  On return, the address of a NSError containing failure information. Otherwise unmodified.
 *  @return     A suitable NSImage that tracks its origins so that the file can be saved without further signal loss from compression */
ASTROLABE_EXTERN NSImage * __nullable
    AstrolabeCompendiumCreateImageFromURL( NSURL * __nullable url,
                                           NSError * __nullable __autoreleasing * __nonnull error) NS_RETURNS_RETAINED;

/*! @abstract Create an image from a URL suitable for saving to a AstrolabeCompendium without loss
 *  @param data      A NSData containing the compressed image
 *  @return     A suitable NSImage that tracks its origins so that the file can be saved without further signal loss from compression */
ASTROLABE_EXTERN NSImage * __nullable
    AstrolabeCompendiumCreateImageFromData( NSData * __nullable data) NS_RETURNS_RETAINED;

/*! @abstract Returns true of a dictionary key contains metadata such as a <template> */
ASTROLABE_EXTERN BOOL
    KeyIsMetadata(id __nullable key);

#endif /* __objc__ */

// Dictionary format
// =================
//
//  The top level object is a dictionary. It does not have a name, except by convention shall be known as <root>.
//      While plist also supports array as the root object, there is little point in that for us, since it would
//      just mean the next level objects were anonymous, known only by their index, which is not helpful.
//  The next (optional) level is a set of named dictionaries, one for each object type (e.g. "Items", "Weapons", "Skills"...)
//      This level, if it occurs, does not have a entry named <template>. When present, the type of object is decoded
//      from these names. Otherwise, it shall be decoded from the file name (e.g. "Items.plist") would contain a list
//      of items.
//  The next N (optional) levels are a set of named dictionaries that subdivide the parent class to subclasses
//      (e.g. Items may contain "Rings", "Jewelry", "Misc Equipment", "Coins"...)
//      These also do not have a <template> entry. These levels and preceding level (without <template>) are merely
//      to provide organization to the namespace of objects -- multiple level namespace, if you will. It also allows
//      some items of the same nominal type to subscribe to different templates. Due to their unusual ranging behavior,
//      for example, one could group shotguns separately from other weapons. Likewise ranged and melee weapons could
//      be grouped separately under separate templates. However, we have not separated ranged and weapons in our
//      implementation because many melee weapons are also thrown ranged weapons.
//  The last level dictionaries are dictionary representations of the (sub)object type.  If, for example, the parent
//      dictionary contains weapons, then all of the objects at this level are the weapons. Each example of the type
//      shall have a consistent set of properties (e.g. damage, HP, attacks, modifiers, base chance to hit, etc.).
//      In addition, there will be a object named <template> which has the same data structure as the other entries 
//      (possibly with extra fields) which serves to enumerate the set of expected fields in the type of object, and
//      the default values for them if a value is absent in the other entries. <template> will also contain an entry
//      called <ToolTips> which reproduces the set of names again, but this time with a helpful human readable
//      descriptions for each.
//
//      For example:
//      ============
//      // skills
//      @{
//          @"Appraise" :   @{  @"Description" : @"Use this skill to estimate an object’s worth, be it an old painting, a gemstone, a weapon, or a riding horse. Failure means they have no idea, while a fumble yields false information. A special or critical yields additional information.",
//                              @"Category" : @"Mental",
//                              @"Base Chance" : @15
//                          },
//          @"Art" :        @{  @"Setting Notes" : @"Art is universal, though it may have era- or setting-specific specialties.",
//                              @"Specialties" : @"Calligraphy, Composing, Conceptual Art, Digital Art, Drawing, Painting, Photography, Poetry, Sculpture, Sketching, Songwriting, Writing, etc.",
//                              @"Description" : @"Use this skill to create a work of art in paint or sculpture, or to compose a story, poem, or musical work. A critical or special success yields a timeless classic of considerably more value and prestige, a failure is mediocre hackwork, and a fumble may harm the creator’s reputation. You would use Perform for acting, dancing, singing, or playing a musical instrument.",
//                              @"Category" : @"Manipulation",
//                              @"Base Chance" : @5
//                          },
//          ...,
//          @"<template>" : @{  @"System Notes" : @"",
//                              @"Setting Notes" : @"",
//                              @"Effects" : @"",
//                              @"Specialties" : @"",
//                              @"Description" : @"",
//                              @"Category" : @"",
//                              @"Base Chance" : @25,
//                              @"<ToolTips>" : @{
//                                  @"System Notes" : @"Relevant rules or systems, if any.",
//                                  @"Setting Notes" : @"Additional information about the skill in different settings. If this entry is not present, the skill works unchanged in almost any setting.",
//                                  @"Effects" : @"The results of a skill roll, for each degree of success, where required. In brief, a skill result is a fumble (the worst possible result, usually a roll of 99 or 00), a failure (a roll above your skill rating), a success (below your skill rating), a special success (1/5 or less of your skill rating), or a critical success (1/20 of your skill rating). Chapter Five: System explains this in detail. See also the Describing Results of Success sidebar (page 38).",
//                                  @"Specialties" : @"A listing of the skill’s distinct specialties, if any. Not all skills with specialties have them elaborated upon in a bulleted list— only those that are more mechanical in nature, such as Combat skills, are detailed in this manner.",
//                                  @"Description" : @"A brief description of the kinds of tasks that can be accomplished using the skill."
//                                  @"Category" : @"The skill category the skill falls into, for purposes of general classification and optional category bonuses. {Combat, Communication, Manipulatiuon, Mental, Preception, Physical}"
//                                  @"Base Chance" : @"The skill rating all characters have in the skill, specialized by era if applicable."
//                              }
//          }
//      }
//
//      The objects in the last level dictionaries (i.e. those with <template>s in them) may be NSString, NSNumber, NSDate,
//      NSAttributedString, NSImage, NSDictionary and NSArray and mutable variants thereof. The keys are all NSStrings.
//      Above this level, all objects are dictionaries.
//
//      If the object is an NSDictionary, it shall also have a corresponding subdictionary entry in the template dictionary
//      associated with the parent, which enumerates what things will be found in tha dictionary object. 
//      Example: A common subdictionary is one called @"Modifiers"," which will contain zero or more NSString : NSString pairs 
//      in the format <variable name> : <expression>. For example: @"STR" : "STR+1", which is identical to the operation 
//      STR = STR + 1. (Think of the colon as an assignment operator.) If a <template> dictionary is present at this level, it 
//      shall have within it a <ToolTips> member that provides human readable help for the object.
//
//      If the template object specifies that a value shall be a NSString, NSNumber, NSDate, NSAttributedString, NSImage, or
//      NSDictionary, the object may either be of that type or a NSArray containing that type. This is occasionally necessary for 
//      things like shotgun damage or energy armor defense values, where a set of values is indicated by the rules rather than a 
//      single expression.
//
//  File format
//  ===========
//      The plist file format is substantially similar, except that NSImage and NSAttributedStrings are replaced by NSData objects.
//      Images and formatted strings are not objects which can be stored as such in a plist, so we wrap them up in a NSData.
//


#ifdef __cplusplus
    }   // extern "C"
#endif /* __cplusplus*/

#endif /* AstrolabeCompendium_h */

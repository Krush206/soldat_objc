/* SharedConfig — Objective-C port of shared/SharedConfig.pas (client
 * branch).
 *
 * Unit-level procedures become class methods of the SharedConfig facade.
 * The SERVER-only LoadBotConfig is not ported. WMName/WMVersion are
 * unit-level vars of Server.pas in the Pascal original; they live here so
 * the client build does not depend on the server unit. */

#ifndef SHAREDCONFIG_H
#define SHAREDCONFIG_H

#import <objc/Object.h>

/* Pascal: WMName, WMVersion: string; (declared in Server.pas) */
extern char *WMName;
extern char *WMVersion;

@interface SharedConfig: Object
+ (BOOL) load_weapons_config: (const char *) filepath;
@end

#endif /* SHAREDCONFIG_H */

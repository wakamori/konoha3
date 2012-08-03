K.import("konoha.new");
/*
 * These methods can be written by following syntax.
 *   grant Admin {
 *     permission "System.f"
 *   }
 */
Security.addRole(new Role("Admin"));
Security.addPermission(new Role("Admin"), "System.static_test");

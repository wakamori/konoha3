K.import("konoha.new");
K.import("konoha.assign");
/*
 * These methods can be written by following syntax.
 *   grant User {
 *     permission "System.f"
 *   }
 *   grant Admin extends User {
 *     permission "System.exec"
 *   }
 */
void define_permissions() {
	Role user = new Role("User");
	Security.addRole(user);
	Security.addPermission(user, "System.f");
	Role admin = new Role("Admin", user);
	Security.addRole(admin);
	Security.addPermission(admin, "System.exec");
}

define_permissions();

package utils.data;

import org.json.JSONException;
import org.json.JSONObject;

public class User {
    private final static String NAME = "name";
    private final String name;
    private final static String ROLE = "role";
    private final UserRole role;

    public User(String name, UserRole role) {
        this.name = name;
        this.role = role;
    }

    public User(JSONObject jsonObject) {
        try {
            name = jsonObject.getString(NAME);
            role = UserRole.valueOf(jsonObject.getString(ROLE));
        } catch (JSONException e) {
            throw new IllegalStateException("No such field: name/role");
        }
    }

    public String getName() {
        return name;
    }

    public UserRole getRole() {
        return role;
    }

    public JSONObject toJSONObject() {
        try {
            JSONObject jsonObject = new JSONObject().put(NAME, name).put(ROLE, role);
            return jsonObject;
        } catch (JSONException e) {
            throw new IllegalStateException(e);
        }
    }
}

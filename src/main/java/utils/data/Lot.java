package utils.data;

import org.json.JSONException;
import org.json.JSONObject;

public class Lot {
    private final static String DESCRIPTION = "description";
    private final String description;
    private final static String CURRENT_PRICE = "currentPrice";
    private int currentPrice;
    private final static String CURRENT_LEADER = "currentLeader";
    private User currentLeader;
    private final static String LOT_ID = "lotId";
    private final int lotId;

    public Lot(String description, int currentPrice, User currentLeader, int lotId) {
        this.description = description;
        this.currentPrice = currentPrice;
        this.currentLeader = currentLeader;
        this.lotId = lotId;
    }

    public Lot(JSONObject jsonObject) {
        try {
            description = jsonObject.getString(DESCRIPTION);
            currentPrice = jsonObject.getInt(CURRENT_PRICE);
            currentLeader = new User(jsonObject.getJSONObject(CURRENT_LEADER));
            lotId = jsonObject.getInt(LOT_ID);
        } catch (JSONException e) {
            throw new IllegalStateException("No such field: description/currentPrice/currentLeader/lotId");
        }
    }

    public Lot(String description, int startValue, int lotId) {
        this(description, startValue, null, lotId);
    }


    public String getDescription() {
        return description;
    }

    public int getCurrentPrice() {
        return currentPrice;
    }

    public User getCurrentLeader() {
        return currentLeader;
    }

    public int getLotId() {
        return lotId;
    }

    public void setCurrentPrice(int currentPrice) {
        this.currentPrice = currentPrice;
    }

    public JSONObject toJSONObject() {
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put(DESCRIPTION, description)
                    .put(CURRENT_PRICE, currentPrice);
            JSONObject currentLeaderJSONObject = currentLeader.toJSONObject();
            jsonObject.put(CURRENT_LEADER, currentLeaderJSONObject)
                    .put(LOT_ID, lotId);
            return jsonObject;
        } catch (JSONException e) {
            throw new IllegalStateException(e);
        }
    }

    public void setCurrentLeader(User currentLeader) {
        this.currentLeader = currentLeader;
    }
}

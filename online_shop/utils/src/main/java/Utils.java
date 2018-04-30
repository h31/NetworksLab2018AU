public class Utils {

    public static final int PORT_SERVER = 8000;
    public static final String ADDRESS_SERVER = "localhost";

    public static final String POST_REQ = "POST";
    public static final String GET_REQ = "GET";

    public class TypeCommand {
        public static final String HELLO = "HELLO";
        public static final String GET = "GET";
        public static final String ADD = "ADD";
        public static final String PAY = "PAY";
        public static final String BYE = "BYE";
        public static final String EXIT = "EXIT";
    }


    public class Status {
        public static final int OK = 0;
        public static final int FAIL = 1;

        public static final int EMPTY_NAME = 2;
    }

    public class JsonElement {
        public static final String NAME = "Name";
        public static final String STATUS = "Status";
        public static final String PRODUCT = "Item";
        public static final String PRODUCTS = "Items";
    }

}

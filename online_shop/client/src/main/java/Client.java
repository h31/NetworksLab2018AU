import org.jetbrains.annotations.NotNull;

public abstract class Client implements Runnable {

    public static final String[] commantType = {
            "Hello in Online Shop",
            "Command list: ",
            " ",
            "HELLO name host port    - start connect with shop serer",
            "GET                     - get all shop product",
            "ADD name_product count  - add product in shop",
            "PAY name_product count  - pay product from shop",
            "BYE                     - stop connection with shop server",
            "EXIT                    - stop application",
            ""
    };


    public static final String[] textCommand = {
            "HELLO kate localhost 8000",
            "GET  ",
            "ADD apple 100",
            "GET",
            "PAY apple 1",
            "GET ",
            "ADD coca 10",
            "GET ",
            "PAY apple 10",
            "GET ",
            "PAY coca 10",
            "GET ",
            "BYE",
            "EXIT"
    };

    abstract void sendGET(@NotNull String[] command);

    abstract void sendHELLO(@NotNull String[] command);

    abstract void sendADD(@NotNull String[] command);

    abstract void sendPAY(@NotNull String[] command);

    abstract void sendBYE(@NotNull String[] command);

    abstract void sendEXIT(@NotNull String[] command);

}

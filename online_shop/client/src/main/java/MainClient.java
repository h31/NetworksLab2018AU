public class MainClient {

    public static void main(String[] args) {
        Client client = new ClientImpl();
        new Thread(client).start();
    }

}

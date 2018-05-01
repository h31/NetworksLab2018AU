import org.jetbrains.annotations.NotNull;

import java.util.HashMap;
import java.util.Map;

public class OnlineShop {

    private final Map<String, Integer> shop = new HashMap<>();


    public synchronized @NotNull Map<String, Integer> getShop() {
        return shop;
    }


    public synchronized void addProductsToShop(Map<String, Integer> products) {
        products.forEach((product, count) -> {
            if (shop.containsKey(product)) {
                shop.put(product, shop.get(product) + count);
            } else {
                shop.put(product, count);
            }
        });
    }

    public void addProductToShop(@NotNull String product, @NotNull Integer count) {
        addProductsToShop(new HashMap<String, Integer>() {{
            put(product, count);
        }});
    }

    public synchronized boolean payProductsFromShop(Map<String, Integer> products) {

        Map<String, Boolean> result = new HashMap<>();
        products.forEach((product, count) -> {
            result.put(product, Boolean.FALSE);
        });

        products.forEach((product, count) -> {

            if (shop.containsKey(product)) {
                shop.put(product, shop.get(product) - count);
                result.put(product, Boolean.TRUE);
            }
        });
        return !result.values().contains(Boolean.FALSE); // pay all position
    }

    public boolean payProductFromShop(@NotNull String product,@NotNull Integer count) {
        return payProductsFromShop(new HashMap<String, Integer>() {{
            put(product, count);
        }});
    }
}

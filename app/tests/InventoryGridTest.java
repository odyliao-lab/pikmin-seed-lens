package dev.ody.seedpreview;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.List;

public final class InventoryGridTest {
    private static final int W = 800, H = 1000, Y = 450;
    private static final int[] CENTERS = {191, 374, 558, 741};

    public static void main(String[] args) throws Exception {
        List<String> refs = Files.readAllLines(Paths.get(args[0]));
        int[] pixels = new int[W * H];
        Arrays.fill(pixels, 0xffe8f8f8);
        require(new InventoryGrid(pixels, H, refs).cards().isEmpty(), "unrelated image");
        for (int x : CENTERS) {
            for (int dy = -23; dy <= 23; dy++) {
                for (int dx = -23; dx <= 23; dx++) {
                    if (dx*dx + dy*dy > 23*23) continue;
                    pixels[(Y+dy)*W+x+dx] = (Math.abs(dx)<=2 && Math.abs(dy)<=8)
                        || (Math.abs(dy)<=2 && Math.abs(dx)<=8) ? 0xffffffff : 0xfffa8256;
                }
            }
        }
        require(new InventoryGrid(pixels, H, refs).cards().isEmpty(), "empty fields");
        String zero = refs.stream().filter(s -> s.startsWith("0 ")).findFirst().get().substring(2);
        draw(pixels, CENTERS[0]-80, Y-152, zero, 0xffb9b9b9);
        List<InventoryGrid.Card> cards = new InventoryGrid(pixels, H, refs).cards();
        require(cards.size()==1 && "0".equals(cards.get(0).petals) && cards.get(0).nectar==null,
                "verified zero and missing nectar must remain distinct");
        Arrays.fill(pixels, (Y-25)*W, (Y+25)*W, 0xffffffff);
        require(new InventoryGrid(pixels, H, refs).cards().isEmpty(), "no anchor, no readings");
        Arrays.fill(pixels, 0xffe8f8f8);
        int clipped = 168;
        for (int x : CENTERS) {
            for (int dy = -23; dy <= 23; dy++) {
                for (int dx = -23; dx <= 23; dx++) {
                    if (dx * dx + dy * dy > 23 * 23) continue;
                    pixels[(clipped + dy) * W + x + dx] = (Math.abs(dx) <= 2 && Math.abs(dy) <= 8)
                        || (Math.abs(dy) <= 2 && Math.abs(dx) <= 8) ? 0xffffffff : 0xfffa8256;
                }
            }
        }
        draw(pixels, CENTERS[0] - 80, clipped - 152, zero, 0xffb9b9b9);
        require(new InventoryGrid(pixels, H, refs).cards().isEmpty(), "clipped card cannot be opened as a crop");
        System.out.println("Inventory grid checks passed");
    }

    private static void draw(int[] pixels, int x, int y, String shape, int ink) {
        for (int dy=0;dy<11;dy++) for (int dx=0;dx<7;dx++) {
            if (shape.charAt(dy*7+dx)=='1') pixels[(y+dy)*W+x+dx]=ink;
        }
    }
    private static void require(boolean ok,String msg) { if (!ok) throw new AssertionError(msg); }
}

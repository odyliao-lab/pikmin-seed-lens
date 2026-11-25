package dev.ody.seedpreview;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/** Coordinates are measured at a normalized 800px width, relative to the four common-nectar badges. */
final class InventoryGrid {
    static final int WIDTH = 800;
    private static final int[] CENTERS = {191, 374, 558, 741};
    private final int[] pixels;
    private final int height;
    private final List<String> templates;

    static final class Card {
        final int left, top;
        final String nectar, petals;
        Card(int left, int top, String nectar, String petals) {
            this.left = left; this.top = top; this.nectar = nectar; this.petals = petals;
        }
    }

    InventoryGrid(int[] pixels, int height, List<String> templates) {
        this.height = height;
        this.pixels = pixels;
        this.templates = templates;
    }

    List<Card> cards() {
        List<Card> cards = new ArrayList<>();
        int first = findBadges();
        if (first >= 0) {
            for (int row = 0; ; row++) {
                int y = first + (int)Math.round(row * 338.5);
                if (y + 87 >= height) break;
                addRow(cards, y, row == 0);
            }
            return cards;
        }
        int header = filterBar();
        if (header < 0) return cards;
        // While scrolling, locate actual petal jars in each full row. Their cork and narrow
        // neck distinguish this menu from the petal selector, whose small icons are nectar.
        for (int y = header + 42; y + 235 < height; y++) {
            int votes = 0;
            for (int x : CENTERS) if (jarAt(x - 124, y)) votes++;
            if (votes < 3) continue;
            int center = printedCenter(y);
            if (center >= 0) addRow(cards, center + 147, false);
            y += 210;
        }
        return cards;
    }

    private void addRow(List<Card> cards, int y, boolean common) {
        // The card and both number bands must fit in the source bitmap.
        if (y < 176 || y + 79 > height) return;
        for (int x : CENTERS) {
            String petals = number(x - 95, x - 17, y - 147, true);
            String nectar = number(x - 114, x + (common ? -25 : 19), y, false);
            if (nectar != null || petals != null) cards.add(new Card(x - 158, y + 35, nectar, petals));
        }
    }

    private int filterBar() {
        for (int y = 30; y < Math.min(height - 40, 420); y++) {
            int yellow = pixels[y * WIDTH + 127], red = pixels[y * WIDTH + 193], blue = pixels[y * WIDTH + 260];
            if (((yellow >> 16) & 255) > 230 && ((yellow >> 8) & 255) > 195 && (yellow & 255) < 100
                    && ((red >> 16) & 255) > 200 && ((red >> 8) & 255) < 160
                    && ((blue >> 16) & 255) < 100 && (blue & 255) > 150) return y;
        }
        return -1;
    }

    private boolean jarAt(int x, int y) {
        return cork(x, y - 18) && cork(x - 2, y - 18)
            && paper(x - 10, y - 18) && paper(x + 10, y - 18) && paper(x, y - 26)
            && !paper(x, y + 15);
    }

    private boolean cork(int x, int y) {
        int c = pixels[y * WIDTH + x], r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
        return r > 80 && r < 245 && r > g + 13 && g > b + 6;
    }

    private boolean paper(int x, int y) {
        int c = pixels[y * WIDTH + x], r = (c >> 16) & 255, g = (c >> 8) & 255;
        return r > 207 && g > 226 && (c & 255) > 223 && g >= r;
    }

    private int printedCenter(int guess) {
        int first = -1, last = -1;
        for (int y = guess - 13; y <= guess + 13; y++) {
            int votes = 0;
            for (int x : CENTERS) {
                for (int i = x - 95; i < x - 17; i++) {
                    if (ink(i, y, true)) { votes++; break; }
                }
            }
            if (votes < 2) continue;
            if (first < 0) first = y;
            last = y;
        }
        return last - first >= 10 && last - first <= 22 ? (first + last) / 2 : -1;
    }

    private int findBadges() {
        // Match a white plus enclosed by orange in each of four columns. A single purchase button is ambiguous.
        for (int y = 165; y < height - 89; y++) {
            boolean found = true;
            for (int x : CENTERS) {
                if (!white(x, y) || !white(x, y - 4) || !white(x + 5, y)
                    || !orange(x - 15, y) || !orange(x + 15, y)
                    || !orange(x, y - 16) || !orange(x, y + 16)) { found = false; break; }
            }
            if (found) {
                int top = y - 16, bottom = y + 16;
                while (top > 0 && orange(CENTERS[0], top - 1)) top--;
                while (bottom + 1 < height && orange(CENTERS[0], bottom + 1)) bottom++;
                if (bottom - top >= 41 && bottom - top <= 50) return (top + bottom + 1) / 2;
            }
        }
        return -1;
    }

    private boolean white(int x, int y) {
        int c = pixels[y * WIDTH + x];
        return ((c >> 16) & 255) > 235 && ((c >> 8) & 255) > 235 && (c & 255) > 235;
    }

    private boolean orange(int x, int y) {
        int c = pixels[y * WIDTH + x], r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
        return r > 230 && g > 90 && g < 160 && b > 45 && b < 120;
    }

    private boolean ink(int x, int y, boolean pale) {
        int c = pixels[y * WIDTH + x], r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
        int hi = Math.max(r, Math.max(g, b)), lo = Math.min(r, Math.min(g, b));
        return pale ? hi - lo < 12 && lo > 155 && hi < 206 : hi - lo < 18 && hi < 137;
    }

    private String number(int left, int right, int center, boolean pale) {
        StringBuilder digits = new StringBuilder();
        int start = -1, top = 25, bottom = -1;
        for (int x = left; x <= right; x++) {
            boolean hasInk = false;
            for (int dy = 0; x < right && dy < 25; dy++) {
                if (!ink(x, center - 12 + dy, pale)) continue;
                hasInk = true;
                top = Math.min(top, dy);
                bottom = Math.max(bottom, dy);
            }
            if (hasInk && start < 0) start = x;
            if (hasInk || start < 0) continue;
            int h = bottom - top + 1;
            if (h >= 10) {
                if (h > 23 || x - start > 22) return null;
                int digit = digit(start, center - 12 + top, x - start, h, pale);
                if (digit < 0) return null;
                digits.append(digit);
            } else if (!(h <= 8 && top >= 15)) {
                return null;
            }
            start = -1; top = 25; bottom = -1;
        }
        if (digits.length() == 0 || digits.length() > 5 ||
                (digits.length() > 1 && digits.charAt(0) == '0')) return null;
        return digits.toString();
    }

    private int digit(int x, int y, int width, int height, boolean pale) {
        char[] shape = new char[77];
        for (int row = 0; row < 11; row++) {
            for (int col = 0; col < 7; col++) {
                int sampleX = x + (2 * col + 1) * width / 14;
                int sampleY = y + (2 * row + 1) * height / 22;
                shape[row * 7 + col] = ink(sampleX, sampleY, pale) ? '1' : '0';
            }
        }
        int[] scores = new int[10];
        Arrays.fill(scores, 78);
        for (String template : templates) {
            int score = 0, n = template.charAt(0) - '0';
            for (int i = 0; i < shape.length; i++) if (template.charAt(i + 2) != shape[i]) score++;
            scores[n] = Math.min(score, scores[n]);
        }
        int best = 0;
        for (int i = 1; i < 10; i++) if (scores[i] < scores[best]) best = i;
        for (int i = 0; i < 10; i++) if (i != best && scores[i] - scores[best] < 3) return -1;
        return scores[best] <= 12 ? best : -1;
    }
}

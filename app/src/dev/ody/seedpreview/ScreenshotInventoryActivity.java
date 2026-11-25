package dev.ody.seedpreview;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ClipData;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.ImageDecoder;
import android.graphics.Insets;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.WindowInsets;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.InterruptedIOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/** Reads one user-selected inventory screenshot. Runs separately from the display controller. */
public final class ScreenshotInventoryActivity extends Activity {
    private static final int PICK_IMAGE = 1;
    private static final int MAX_IMAGE_BYTES = 16 * 1024 * 1024;
    private static final String SELECTED = "selected";
    private final ExecutorService worker = Executors.newSingleThreadExecutor();
    private ForestStyle style;
    private LinearLayout results;
    private TextView status;
    private Button choose;
    private Uri selected;

    @Override protected void onCreate(Bundle state) {
        super.onCreate(state);
        build();
        if (state != null && state.getParcelable(SELECTED) instanceof Uri) {
            read((Uri) state.getParcelable(SELECTED));
        } else if (Intent.ACTION_SEND.equals(getIntent().getAction())) {
            try {
                read(sharedImage(getIntent()));
            } catch (RuntimeException invalidShare) {
                status.setText("無法讀取分享的圖片。請用選圖按鈕重新選取。");
            }
        }
    }

    private static Uri sharedImage(Intent intent) {
        if ((intent.getFlags() & Intent.FLAG_GRANT_READ_URI_PERMISSION) == 0) return null;
        Object stream = intent.getParcelableExtra(Intent.EXTRA_STREAM);
        if (stream != null && !(stream instanceof Uri)) return null;
        ClipData clip = intent.getClipData();
        if (clip == null) return (Uri) stream;
        if (clip.getItemCount() != 1) return null;
        Uri uri = clip.getItemAt(0).getUri();
        return stream == null || stream.equals(uri) ? uri : null;
    }

    private void build() {
        boolean dark = (getResources().getConfiguration().uiMode & Configuration.UI_MODE_NIGHT_MASK)
            == Configuration.UI_MODE_NIGHT_YES;
        style = new ForestStyle(this, dark);
        LinearLayout screen = style.column();
        screen.setBackgroundColor(style.paper);
        if (Build.VERSION.SDK_INT >= 30) getWindow().setDecorFitsSystemWindows(false);
        getWindow().getDecorView().setSystemUiVisibility(dark ? 0 :
            View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR | View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
        screen.setOnApplyWindowInsetsListener((v, insets) -> {
            if (Build.VERSION.SDK_INT >= 30) {
                Insets bars = insets.getInsets(WindowInsets.Type.systemBars());
                v.setPadding(bars.left, bars.top, bars.right, bars.bottom);
            } else {
                v.setPadding(insets.getSystemWindowInsetLeft(), insets.getSystemWindowInsetTop(),
                    insets.getSystemWindowInsetRight(), insets.getSystemWindowInsetBottom());
            }
            return insets;
        });
        LinearLayout header = style.column();
        header.setPadding(style.dp(16), style.dp(8), style.dp(16), 0);
        LensTabs.addInventory(this, style, header);
        style.add(screen, header, 0);

        ScrollView scroll = new ScrollView(this);
        screen.addView(scroll, new LinearLayout.LayoutParams(-1, 0, 1));
        LinearLayout page = style.column();
        page.setPadding(style.dp(16), style.dp(6), style.dp(16), style.dp(16));
        scroll.addView(page);
        style.add(page, style.heading("精華與花瓣"), 8);
        style.add(page, style.text("截圖辨識 · Pikmin Bloom 158.0", 14, style.green, true), 10);
        style.add(page, style.text("在隊伍畫面點精華球旁的數量，截下完整直式清單，再選圖或分享到透視鏡。圖片只在本機辨識。", 15, style.ink, false), 12);
        choose = style.button("選取清單截圖", true);
        choose.setOnClickListener(v -> startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT)
            .addCategory(Intent.CATEGORY_OPENABLE).setType("image/*"), PICK_IMAGE));
        style.add(page, choose, 12);
        status = style.text("", 14, style.muted, false);
        status.setAccessibilityLiveRegion(View.ACCESSIBILITY_LIVE_REGION_POLITE);
        style.add(page, status, 8);
        results = style.column();
        style.add(page, results, 0);
        setContentView(screen);
        screen.requestApplyInsets();
    }

    @Override protected void onActivityResult(int request, int result, Intent data) {
        super.onActivityResult(request, result, data);
        if (request == PICK_IMAGE && result == RESULT_OK && data != null) read(data.getData());
    }

    @Override protected void onSaveInstanceState(Bundle out) {
        super.onSaveInstanceState(out);
        if (selected != null) out.putParcelable(SELECTED, selected);
    }

    private void read(Uri uri) {
        results.removeAllViews();
        selected = null;
        if (uri == null || !"content".equals(uri.getScheme())) {
            status.setText("無法讀取這張圖片。請使用系統選圖器或重新分享。");
            return;
        }
        selected = uri;
        choose.setEnabled(false);
        status.setText("正在辨識截圖…");
        worker.execute(() -> {
            try {
                Bitmap image = decode(uri);
                List<String> templates = new ArrayList<>();
                try (BufferedReader input = new BufferedReader(new InputStreamReader(
                        getAssets().open("inventory158-digits.tsv"), StandardCharsets.US_ASCII))) {
                    String line;
                    while ((line = input.readLine()) != null) templates.add(line);
                }
                int width = image.getWidth();
                int height = image.getHeight();
                int[] pixels = new int[width * height];
                image.getPixels(pixels, 0, width, 0, 0, width, height);
                List<InventoryGrid.Card> cards = new InventoryGrid(pixels, height, templates).cards();
                runOnUiThread(() -> present(image, cards));
            } catch (IOException | RuntimeException ignored) {
                runOnUiThread(() -> {
                    if (isDestroyed() || isFinishing()) return;
                    selected = null;
                    choose.setEnabled(true);
                    status.setText("無法讀取圖片。請選用完整直式 PNG、JPEG 或 WebP 截圖（16 MB 以內）。");
                });
            }
        });
    }

    private Bitmap decode(Uri uri) throws IOException {
        // Bound encoded bytes even when a document provider supplies no size.
        try (InputStream input = getContentResolver().openInputStream(uri);
                ByteArrayOutputStream data = new ByteArrayOutputStream()) {
            if (input == null) throw new IOException("Image unavailable");
            byte[] buffer = new byte[32 * 1024];
            int count;
            while ((count = input.read(buffer)) != -1) {
                if (Thread.currentThread().isInterrupted()) throw new InterruptedIOException();
                if (count > MAX_IMAGE_BYTES - data.size()) throw new IOException("Image too large");
                data.write(buffer, 0, count);
            }
            return ImageDecoder.decodeBitmap(ImageDecoder.createSource(ByteBuffer.wrap(data.toByteArray())),
                (decoder, info, source) -> {
                    String mime = info.getMimeType();
                    int width = info.getSize().getWidth();
                    int height = info.getSize().getHeight();
                    if (info.isAnimated() ||
                            !("image/png".equals(mime) || "image/jpeg".equals(mime) || "image/webp".equals(mime))
                            || width < 540 || height <= width || height > 4L * width
                            || (long) width * height > 24_000_000) {
                        throw new IllegalArgumentException("Unsupported screenshot");
                    }
                    decoder.setAllocator(ImageDecoder.ALLOCATOR_SOFTWARE);
                    decoder.setTargetSize(InventoryGrid.WIDTH,
                        (int) Math.round((double) InventoryGrid.WIDTH * height / width));
                });
        }
    }

    private void present(Bitmap image, List<InventoryGrid.Card> cards) {
        if (isDestroyed() || isFinishing()) return;
        choose.setEnabled(true);
        if (cards.isEmpty()) {
            status.setText("未辨識到清單。請保留上方篩選列及至少一整列花種，再試一次。");
            return;
        }
        status.setText("辨識到 " + cards.size() + " 張卡片。點卡片查看原圖。");
        for (InventoryGrid.Card card : cards) {
            LinearLayout row = new LinearLayout(this);
            row.setGravity(Gravity.CENTER_VERTICAL);
            row.setPadding(style.dp(8), style.dp(6), style.dp(8), style.dp(6));
            row.setBackground(style.panel(style.surface, 12, true));
            ImageView name = new ImageView(this);
            name.setImageBitmap(Bitmap.createBitmap(image, card.left - 2, card.top, 182, 44));
            name.setImportantForAccessibility(View.IMPORTANT_FOR_ACCESSIBILITY_NO);
            row.addView(name, new LinearLayout.LayoutParams(style.dp(105), style.dp(40)));
            String counts = "精華 " + value(card.nectar) + "\n花瓣 " + value(card.petals);
            TextView summary = style.text(counts, 15, style.ink, false);
            summary.setPadding(style.dp(10), 0, 0, 0);
            row.addView(summary, new LinearLayout.LayoutParams(0, -2, 1));
            row.setContentDescription(counts + "，查看卡片原圖");
            row.setFocusable(true);
            row.setOnClickListener(v -> {
                ImageView source = new ImageView(this);
                source.setAdjustViewBounds(true);
                source.setImageBitmap(Bitmap.createBitmap(image, card.left - 2, card.top - 211, 182, 255));
                new AlertDialog.Builder(this).setTitle("截圖中的卡片").setView(source)
                    .setPositiveButton("關閉", null).show();
            });
            style.add(results, row, 6);
        }
    }

    private static String value(String number) {
        return number == null ? "待核對" : number;
    }

    @Override protected void onDestroy() {
        worker.shutdownNow();
        super.onDestroy();
    }
}

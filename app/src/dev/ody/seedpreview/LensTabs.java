package dev.ody.seedpreview;

import android.app.Activity;
import android.content.Intent;
import android.view.Gravity;
import android.widget.Button;
import android.widget.LinearLayout;

final class LensTabs {
    static void add(Activity activity, ForestStyle style, LinearLayout page, boolean diagnostic) {
        add(activity, style, page, diagnostic, false);
    }

    static void add(Activity activity, ForestStyle style, LinearLayout page, boolean diagnostic, boolean compact) {
        addTabs(activity, style, page, diagnostic ? 2 : 0, compact);
    }

    static void addInventory(Activity activity, ForestStyle style, LinearLayout page) {
        addTabs(activity, style, page, 1, true);
    }

    private static void addTabs(Activity activity, ForestStyle style, LinearLayout page, int active, boolean compact) {
        String[] labels = {"工具", "截圖庫存", "診斷"};
        Class<?>[] screens = {MainActivity.class, ScreenshotInventoryActivity.class, DiagnosticsActivity.class};
        LinearLayout row = new LinearLayout(activity);
        for (int i = 0; i < labels.length; i++) {
            boolean selected = i == active;
            Class<?> screen = screens[i];
            Button button = style.button(labels[i], selected);
            button.setSelected(selected);
            if (compact) {
                button.setMinHeight(style.dp(48));
                button.setMinimumHeight(style.dp(48));
                button.setPadding(style.dp(6), style.dp(6), style.dp(6), style.dp(6));
            }
            button.setGravity(Gravity.CENTER);
            row.addView(button, new LinearLayout.LayoutParams(0, -2, 1));
            button.setOnClickListener(v -> {
                if (selected) return;
                activity.startActivity(new Intent(activity, screen).addFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT));
                if (active != 0) activity.finish();
            });
        }
        style.add(page, row, compact ? 8 : 12);
    }
}

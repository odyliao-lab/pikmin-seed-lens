package dev.ody.seedpreview;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

/** Read the immutable illustration from this installed APK, without a network or cache. */
final class OfflineArtwork {
    static Bitmap mascot(Context context) throws IOException {
        // A flat packaged name avoids Windows aapt2's nested-asset separator
        // issue. Native assets and their security checks use their own paths.
        try (InputStream in = context.getAssets().open("forest-lens-mascot-v1.png")) {
            ByteArrayOutputStream bytes = new ByteArrayOutputStream();
            byte[] buffer = new byte[16384];
            int count;
            while ((count = in.read(buffer)) != -1) {
                if (bytes.size() + count > 4 * 1024 * 1024)
                    throw new IOException("Oversized offline illustration");
                bytes.write(buffer, 0, count);
            }
            byte[] png = bytes.toByteArray();
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inSampleSize = 2;
            Bitmap result = BitmapFactory.decodeByteArray(png, 0, png.length, options);
            if (result == null) throw new IOException("Invalid offline illustration");
            return result;
        }
    }
}

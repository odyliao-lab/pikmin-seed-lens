import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "花苗透視鏡｜問題回報",
  description: "自願診斷回報與私人管理後台",
  icons: {
    icon: "/favicon.svg",
    shortcut: "/favicon.svg",
  },
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="zh-Hant">
      <body className="antialiased">{children}</body>
    </html>
  );
}

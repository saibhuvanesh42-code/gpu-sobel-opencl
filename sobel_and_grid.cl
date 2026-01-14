void sobel_and_grid_cpu(
    unsigned char* img,
    int width,
    int height
) {
    std::vector<unsigned char> copy(img, img + width * height * 3);

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {

            int idx = (y * width + x) * 3;

            int gray[9];
            int k = 0;

            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    int nidx = ((y + j) * width + (x + i)) * 3;
                    gray[k++] = (copy[nidx] + copy[nidx + 1] + copy[nidx + 2]) / 3;
                }
            }

            int gx =
                -gray[0] + gray[2]
                -2 * gray[3] + 2 * gray[5]
                -gray[6] + gray[8];

            int gy =
                -gray[0] - 2 * gray[1] - gray[2]
                +gray[6] + 2 * gray[7] + gray[8];

            int magnitude = abs(gx) + abs(gy);

            if (magnitude > 150) {
                img[idx + 0] = 255;
                img[idx + 1] = 0;
                img[idx + 2] = 0;
            }

            if ((x % 20 < 3) || (y % 20 < 3)) {
                img[idx + 0] = 255;
                img[idx + 1] = 0;
                img[idx + 2] = 0;
            }
        }
    }
}

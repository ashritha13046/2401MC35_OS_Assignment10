#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_REF 30
#define MAX_FRAMES 10

int ref_str[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
int ref_len = 20;

void print_step(int ref, int frames[], int num_frames, int is_fault) {
  printf(1, "Ref %d | Frames: [", ref);
  for (int i = 0; i < num_frames; i++) {
    if (frames[i] == -1)
      printf(1, " -");
    else
      printf(1, " %d", frames[i]);
  }
  printf(1, " ] | %s\n", is_fault ? "FAULT" : "HIT");
}

void fifo(int num_frames, int print_details, int *faults, int *hits) {
  int frames[MAX_FRAMES];
  for (int i = 0; i < num_frames; i++) frames[i] = -1;

  *faults = 0;
  *hits = 0;
  int fifo_ptr = 0;

  if (print_details)
    printf(1, "\n--- FIFO Page Replacement (%d Frames) ---\n", num_frames);

  for (int i = 0; i < ref_len; i++) {
    int page = ref_str[i];
    int found = 0;

    for (int j = 0; j < num_frames; j++) {
      if (frames[j] == page) {
        found = 1;
        break;
      }
    }

    if (found) {
      (*hits)++;
      if (print_details) print_step(page, frames, num_frames, 0);
    } else {
      (*faults)++;
      frames[fifo_ptr] = page;
      fifo_ptr = (fifo_ptr + 1) % num_frames;
      if (print_details) print_step(page, frames, num_frames, 1);
    }
  }
}

void lru(int num_frames, int print_details, int *faults, int *hits) {
  int frames[MAX_FRAMES];
  int last_used[MAX_FRAMES];
  for (int i = 0; i < num_frames; i++) {
    frames[i] = -1;
    last_used[i] = -1;
  }

  *faults = 0;
  *hits = 0;

  if (print_details)
    printf(1, "\n--- LRU Page Replacement (%d Frames) ---\n", num_frames);

  for (int i = 0; i < ref_len; i++) {
    int page = ref_str[i];
    int found = 0;

    for (int j = 0; j < num_frames; j++) {
      if (frames[j] == page) {
        found = 1;
        last_used[j] = i;
        break;
      }
    }

    if (found) {
      (*hits)++;
      if (print_details) print_step(page, frames, num_frames, 0);
    } else {
      (*faults)++;
      int lru_idx = 0;
      for (int j = 1; j < num_frames; j++) {
        if (frames[j] == -1) {
          lru_idx = j;
          break;
        }
        if (last_used[j] < last_used[lru_idx]) {
          lru_idx = j;
        }
      }
      frames[lru_idx] = page;
      last_used[lru_idx] = i;
      if (print_details) print_step(page, frames, num_frames, 1);
    }
  }
}

void optimal(int num_frames, int print_details, int *faults, int *hits) {
  int frames[MAX_FRAMES];
  for (int i = 0; i < num_frames; i++) frames[i] = -1;

  *faults = 0;
  *hits = 0;

  if (print_details)
    printf(1, "\n--- OPTIMAL Page Replacement (%d Frames) ---\n", num_frames);

  for (int i = 0; i < ref_len; i++) {
    int page = ref_str[i];
    int found = 0;

    for (int j = 0; j < num_frames; j++) {
      if (frames[j] == page) {
        found = 1;
        break;
      }
    }

    if (found) {
      (*hits)++;
      if (print_details) print_step(page, frames, num_frames, 0);
    } else {
      (*faults)++;
      int replace_idx = -1;
      int empty_idx = -1;

      for (int j = 0; j < num_frames; j++) {
        if (frames[j] == -1) {
          empty_idx = j;
          break;
        }
      }

      if (empty_idx != -1) {
        replace_idx = empty_idx;
      } else {
        int farthest = i;
        for (int j = 0; j < num_frames; j++) {
          int next_use;
          for (next_use = i + 1; next_use < ref_len; next_use++) {
            if (ref_str[next_use] == frames[j])
              break;
          }
          if (next_use > farthest) {
            farthest = next_use;
            replace_idx = j;
          }
        }
      }

      frames[replace_idx] = page;
      if (print_details) print_step(page, frames, num_frames, 1);
    }
  }
}

int
main(void)
{
  int faults, hits;

  printf(1, "==== Q4: PAGE REPLACEMENT ALGORITHMS ====\n");
  printf(1, "Reference String: 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1\n");

  // Task b: Display step-by-step for 3 frames
  fifo(3, 1, &faults, &hits);
  printf(1, "Total FIFO Page Faults (3 frames): %d\n", faults);

  lru(3, 1, &faults, &hits);
  printf(1, "Total LRU Page Faults (3 frames): %d\n", faults);

  optimal(3, 1, &faults, &hits);
  printf(1, "Total Optimal Page Faults (3 frames): %d\n", faults);

  // Task c: Comparison Table across 3, 4, 5 frames
  printf(1, "\n=========================================================================\n");
  printf(1, "                              COMPARISON TABLE                           \n");
  printf(1, "=========================================================================\n");
  printf(1, "Algo    | Frames | Page Faults | Page Hits | Hit Ratio (%%) | Fault Ratio (%%)\n");
  printf(1, "--------+--------+-------------+-----------+---------------+----------------\n");

  int frame_counts[] = {3, 4, 5};

  for (int f = 0; f < 3; f++) {
    int fr = frame_counts[f];

    fifo(fr, 0, &faults, &hits);
    printf(1, "FIFO    |   %d    |     %d      |    %d     |     %d%%       |      %d%%\n",
           fr, faults, hits, (hits * 100) / ref_len, (faults * 100) / ref_len);

    lru(fr, 0, &faults, &hits);
    printf(1, "LRU     |   %d    |     %d      |    %d     |     %d%%       |      %d%%\n",
           fr, faults, hits, (hits * 100) / ref_len, (faults * 100) / ref_len);

    optimal(fr, 0, &faults, &hits);
    printf(1, "OPTIMAL |   %d    |     %d      |    %d     |     %d%%       |      %d%%\n",
           fr, faults, hits, (hits * 100) / ref_len, (faults * 100) / ref_len);
    printf(1, "--------+--------+-------------+-----------+---------------+----------------\n");
  }

  printf(1, "\nExplanation: OPTIMAL performs best because it replaces the page that will\n");
  printf(1, "not be needed for the longest period in the future, minimizing total faults.\n");

  exit();
}
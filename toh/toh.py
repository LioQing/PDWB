import pygame
import functools
import time
from collections.abc import Generator
from collections import Sequence
from typing import Union

screen = pygame.display.set_mode((800, 600))
disk_size = (200, 50)
spacing = (20, 5)
height = 3
speed = (3, 15)


def init(max_disk: int, towers: Sequence[Sequence[int]], animation_speed: float = 15):
    """
    Initialize the display

    :param max_disk: max number of elements in each queue
    :param towers: the towers to be displayed
    :param animation_speed: the speed of the animation
    """
    global screen
    global height
    global speed

    speed = (animation_speed / 5, animation_speed)

    pygame.init()
    pygame.display.set_caption("Tower of Hanoi")
    height = max_disk
    screen = pygame.display.set_mode(
        (len(towers) * (disk_size[0] + spacing[0]) + spacing[0],
         (max_disk + 1) * (disk_size[1] + spacing[1]) + spacing[1]))
    pygame.fastevent.init()

    _event_handling()
    screen.fill((0, 0, 0))
    pygame.display.update()


def finish():
    """
    Finish the display
    """
    while True:
        if _event_handling():
            break

        pygame.display.update()


def draw(towers: Sequence[Sequence[int]], pop_from: Union[int, Sequence[int]] = None,
         push_to: Union[int, Sequence[int]] = None):
    """
    Draw the current state of the towers

    :param towers: the towers to be displayed
    :param pop_from: the index of the tower from which an element was popped
    :param push_to: the index of the tower to which an element was pushed
    """
    global screen

    if pop_from is not None and type(pop_from) is not int:
        pop_from = towers.index(pop_from)

    if push_to is not None and type(push_to) is not int:
        push_to = towers.index(push_to)

    if push_to is not None and len(towers[push_to]) <= 0:
        raise ValueError("Tower to push to is empty")

    disk_lengths = tuple(tuple(d) for d in _get_disk_lengths(towers))

    if pop_from is not None and push_to is not None:
        _animate_pop_push(towers, disk_lengths, pop_from, push_to)

    _event_handling()

    screen.fill((0, 0, 0))
    for i, (tower, disks) in enumerate(zip(towers, disk_lengths)):
        _draw_tower(i, tower, disks)

    pygame.display.update()


def _animate_pop_push(towers: Sequence[Sequence[int]],
                      disk_lengths: Sequence[Sequence[int, None, None], None, None],
                      pop_from: int,
                      push_to: int):
    """
    Animate the pop and push of an element from one tower to another

    :param towers: the towers to be displayed
    :param disk_lengths: the lengths of the disks in the towers
    :param pop_from: the index of the tower from which an element was popped
    :param push_to: the index of the tower to which an element was pushed
    """
    global screen

    arrived = False
    start_time = time.time()
    delta_time = 0.

    pos = [pop_from, len(towers[pop_from])]
    stage = 0
    destination, direction = _path(pop_from, push_to, len(towers[push_to]))[stage]

    while not arrived:
        _event_handling()

        pos[0] += direction[0] * speed[0] * delta_time
        pos[1] += direction[1] * speed[1] * delta_time

        screen.fill((0, 0, 0))
        for i, (tower, disks) in enumerate(zip(towers, disk_lengths)):
            if i == push_to:
                _draw_tower(i, tower[:-1], disks[:-1])
            else:
                _draw_tower(i, tower, disks)

        if direction[0] > 0 and pos[0] >= destination[0] or direction[0] < 0 and pos[0] <= destination[0] or \
                direction[1] > 0 and pos[1] >= destination[1] or direction[1] < 0 and pos[1] <= destination[1]:
            pos = list(destination)
            stage += 1

            if stage == 3:
                arrived = True
            else:
                destination, direction = _path(pop_from, push_to, len(towers[push_to]) - 1)[stage]

        _draw_disk_centered(screen, disk_lengths[push_to][-1], *_get_pos_from_index(*pos))

        pygame.display.update()

        end_time = time.time()
        delta_time = end_time - start_time
        start_time = end_time


def _path(pop_from: int, push_to: int, push_to_height: int) -> list[(int, int)]:
    """
    Get the path of the disk from the pop tower to the push tower

    :param pop_from: the index of the tower from which an element was popped
    :param push_to: the index of the tower to which an element was pushed
    :return: destination and direction to move in
    """
    return [
        ((pop_from, height), (0, 1)),
        ((push_to, height), (1 if pop_from < push_to else -1, 0)),
        ((push_to, push_to_height), (0, -1))
    ]


def _get_pos_from_index(i: float, j: float) -> (int, int):
    """
    Get the position of the disk from the index of the tower and the height of the disk

    :param i: index of the tower
    :param j: height of the disk
    :return: the position of the disk
    """

    return spacing[0] + i * (disk_size[0] + spacing[0]) + disk_size[0] // 2, (height - j) * (
            disk_size[1] + spacing[1]) + disk_size[1] // 2


def _draw_tower(i: int, tower: Sequence[int], disks: Union[Generator[int, None, None], Sequence[int]]):
    """
    Draw a tower

    :param i: the index of the tower
    :param tower: the tower to be drawn
    :param disks: the lengths of the disks in the tower
    """
    for j, (disk, length) in enumerate(zip(tower, disks)):
        _draw_disk_centered(screen, length, *_get_pos_from_index(i, j))


def _event_handling() -> bool:
    """
    Handle events
    """
    for event in pygame.fastevent.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            return True
    return False


def _draw_disk_centered(surface: pygame.Surface, length: int, x: int, y: int):
    """
    Draw a disk with the given length centered at the given position

    :param surface: the surface on which to draw the disk
    :param length: the length of the disk
    :param x: the x coordinate of the center of the disk
    :param y: the y coordinate of the center of the disk
    """
    pygame.draw.rect(surface, (255, 255, 255), (x - length / 2, y - 10, length, disk_size[1]))


def _get_disk_lengths(towers: Sequence[Sequence[int]]) -> Generator[Generator[int, None, None], None, None]:
    """
    Get the lengths of the disks in the given towers

    :param towers: the towers to get the disk lengths from
    :return: a generator of generators of the disk lengths
    """
    max_disk = max(max(tower) if len(tower) > 0 else 1 for tower in towers)
    return ((disk * disk_size[0] // max_disk for disk in tower) for tower in towers)
